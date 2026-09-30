#!/usr/bin/env python3
"""Resolve and validate an Android compatibility profile before replacing locks.

Libraries follow latest stable on first resolution / explicit update. Toolchain
versions form a reviewed profile: upgrading AGP does not independently upgrade
Gradle, Kotlin, NDK, or SDK. No SDK or JDK is installed by this script.
"""
from __future__ import annotations

import argparse
from contextlib import contextmanager
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import urllib.request
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[2]
ANDROID = ROOT / "Workbench/platform/android"
GOOGLE = "https://dl.google.com/dl/android/maven2/"
ARTIFACTS = {
    "agp": "com/android/tools/build/gradle",
    "compose_bom": "androidx/compose/compose-bom",
    "activity": "androidx/activity/activity-compose",
    "lifecycle": "androidx/lifecycle/lifecycle-runtime-compose",
}
LOCK_FILES = (
    "android-dependencies.json", "gradle/wrapper/gradle-wrapper.properties",
    "gradle/verification-metadata.xml", "buildscript-gradle.lockfile",
    "app/gradle.lockfile",
)


def read_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def fetch(url):
    with urllib.request.urlopen(url, timeout=60) as response:
        if not response.url.startswith("https://"):
            raise ValueError("Insecure dependency redirect")
        return response.read()


def stable_versions(data):
    versions = [v.text for v in ET.fromstring(data).findall("./versioning/versions/version")]
    return sorted((v for v in versions if v and re.fullmatch(r"\d+(?:\.\d+)+", v)),
                  key=lambda v: tuple(map(int, v.split("."))))


def requests(manifest, profile_name, overrides, environ):
    if manifest.get("schema") != 2 or not isinstance(manifest.get("profile"), str) or not isinstance(manifest.get("versions", {}), dict):
        raise ValueError("Unsupported Android dependency manifest schema")
    selected = dict(manifest.get("versions", {}))
    for name in (*ARTIFACTS, "kotlin", "gradle", "compile_sdk", "build_tools", "ndk", "cmake", "java"):
        value = environ.get("ARIA_DEP_ANDROID_" + name.upper() + "_VERSION")
        if value:
            selected[name] = value
    selected.update(overrides)
    valid = set(ARTIFACTS) | {"kotlin", "gradle", "compile_sdk", "build_tools", "ndk", "cmake", "java"}
    for name, value in selected.items():
        if name not in valid or not isinstance(value, str) or not re.fullmatch(r"\d+(?:\.\d+)*", value) or (name in ("compile_sdk", "java") and not value.isdigit()):
            raise ValueError(f"Invalid explicit Android version: {name}={value}")
    return {"profile": profile_name or environ.get("ARIA_ANDROID_PROFILE") or manifest["profile"],
            "versions": selected}


def choose_versions(request, profiles, download=fetch):
    profile = profiles.get("profiles", {}).get(request["profile"])
    if not profile:
        raise ValueError(f"Unknown reviewed Android profile: {request['profile']}")
    versions = {k: v for k, v in profile.items() if k not in ("agp_series", "references")}
    for name, coordinate in ARTIFACTS.items():
        available = stable_versions(download(GOOGLE + coordinate + "/maven-metadata.xml"))
        explicit = request["versions"].get(name)
        if explicit:
            if explicit not in available:
                raise ValueError(f"Android dependency {name} has no stable release {explicit}")
            versions[name] = explicit
        else:
            candidates = [v for v in available if name != "agp" or v.startswith(profile["agp_series"])]
            if not candidates:
                raise ValueError(f"No stable {name} release in profile {request['profile']}")
            versions[name] = candidates[-1]
    versions.update(request["versions"])
    if not versions["agp"].startswith(profile["agp_series"]):
        raise ValueError("AGP override must belong to the selected compatibility profile")
    if tuple(map(int, versions["gradle"].split("."))) < tuple(map(int, profile["gradle"].split("."))):
        raise ValueError("Gradle override is below this profile's minimum version")
    return versions


def platform_package(compile_sdk):
    # Android 17 introduced the major.minor platform package naming.
    return "android-" + compile_sdk + (".0" if int(compile_sdk) >= 37 else "")


def fingerprint(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def profile_digest(resolved):
    return fingerprint({name: resolved[name] for name in ("request_hash", "profile_hash", "versions")})


@contextmanager
def project_lock(project):
    identity = hashlib.sha256(os.path.normcase(str(project.resolve())).encode()).hexdigest()
    path = Path(tempfile.gettempdir()) / ("aria-android-update-" + identity + ".lock")
    with path.open("a+b") as handle:
        handle.write(b"0")
        handle.flush()
        handle.seek(0)
        try:
            if os.name == "nt":
                import msvcrt
                msvcrt.locking(handle.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(handle.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as error:
            raise ValueError("Another Android dependency update is already running for this project") from error
        try:
            yield
        finally:
            handle.seek(0)
            if os.name == "nt":
                msvcrt.locking(handle.fileno(), msvcrt.LK_UNLCK, 1)
            else:
                fcntl.flock(handle.fileno(), fcntl.LOCK_UN)


def check_lock(project, request, profiles):
    document = read_json(project / LOCK_FILES[0])
    lock = document.get("resolved", {})
    if document.get("schema") != 2 or lock.get("request_hash") != fingerprint(request):
        raise ValueError("Android dependency requests changed; run android_dependencies.py resolve")
    profile = profiles["profiles"].get(request["profile"])
    if lock.get("profile_hash") != fingerprint(profile):
        raise ValueError("Android compatibility profile changed; resolve the profile again")
    versions = lock.get("versions")
    required = set(ARTIFACTS) | {"kotlin", "gradle", "compile_sdk", "build_tools", "ndk", "cmake", "java"}
    if not isinstance(versions, dict) or set(versions) != required:
        raise ValueError("Android lock has no complete selected versions")
    requests({"schema": 2, "profile": request["profile"], "versions": versions}, None, {}, {})
    if any(versions.get(name) != value for name, value in request["versions"].items()):
        raise ValueError("Android lock does not honor explicit version requests")
    if lock.get("validated_profile_sha256") != profile_digest(lock):
        raise ValueError("Android selected profile changed after validation")
    expected = lock.get("validated_files", {})
    if set(expected) != set(LOCK_FILES[1:]):
        raise ValueError("Android lock has no complete Gradle validation record")
    for name, sha in expected.items():
        if digest(project / name) != sha:
            raise ValueError(f"Android validated lock file changed: {name}; resolve again")
    return lock


def seed_aapt2_platforms(project, download=fetch):
    """Record official aapt2 classifier hashes for all supported CI hosts."""
    path = project / "gradle/verification-metadata.xml"
    ns = "{https://schema.gradle.org/dependency-verification}"
    tree = ET.parse(path)
    for component in tree.getroot().findall(f"{ns}components/{ns}component"):
        if component.get("group") != "com.android.tools.build" or component.get("name") != "aapt2":
            continue
        version = component.attrib["version"]
        names = {item.get("name") for item in component.findall(f"{ns}artifact")}
        for host in ("linux", "windows", "osx"):
            name = f"aapt2-{version}-{host}.jar"
            if name not in names:
                data = download(GOOGLE + f"com/android/tools/build/aapt2/{version}/{name}")
                artifact = ET.SubElement(component, ns + "artifact", {"name": name})
                ET.SubElement(artifact, ns + "sha256", {"value": hashlib.sha256(data).hexdigest(),
                                                      "origin": "Official Google Maven artifact"})
    ET.register_namespace("", ns[1:-1])
    ET.indent(tree, space="   ")
    tree.write(path, encoding="utf-8", xml_declaration=True)


def validate_candidate(project, native_root, versions, request, profiles):
    sdk = os.environ.get("ANDROID_SDK_ROOT") or os.environ.get("ANDROID_HOME")
    if not sdk:
        raise ValueError("Set ANDROID_SDK_ROOT to an installed SDK; no SDK is installed automatically")
    needed = [f"platforms/{platform_package(versions['compile_sdk'])}", f"build-tools/{versions['build_tools']}",
              f"ndk/{versions['ndk']}", f"cmake/{versions['cmake']}", "platform-tools"]
    missing = [item for item in needed if not (Path(sdk) / item).is_dir()]
    if missing:
        raise ValueError("Selected Android profile requires installed SDK packages: " + ", ".join(missing))
    native_build = native_root / "build/platforms/android"
    for required in ("include/nlohmann/json.hpp", "aria-source.txt", "lib/libwb_core_app.a"):
        if not (native_build / required).is_file():
            raise ValueError(f"Native export {required} is missing; run gen-android.sh first")
    native_cache = native_build / "CMakeCache.txt"
    if not native_cache.is_file():
        raise ValueError("Build the native core with gen-android.sh before resolving Android libraries")
    ndk_path = next((line.split("=", 1)[1] for line in native_cache.read_text().splitlines()
                     if line.startswith(("CMAKE_ANDROID_NDK:", "ANDROID_NDK:"))), None)
    if not ndk_path:
        toolchain = next((line.split("=", 1)[1] for line in native_cache.read_text().splitlines()
                          if line.startswith("CMAKE_TOOLCHAIN_FILE:")), None)
        if toolchain and Path(toolchain).name == "android.toolchain.cmake":
            ndk_path = str(Path(toolchain).resolve().parents[2])
    if not ndk_path:
        raise ValueError("Native core cache has no selected NDK; rebuild it with gen-android.sh")
    properties = (Path(ndk_path) / "source.properties").read_text()
    revision = re.search(r"^Pkg.Revision\s*=\s*(\S+)", properties, re.MULTILINE)
    if not revision or revision.group(1) != versions["ndk"]:
        raise ValueError("Native core NDK differs from the requested profile; rebuild it with matching ARIA_DEP_ANDROID_NDK_VERSION")
    profile_lock = {"request_hash": fingerprint(request),
                    "profile_hash": fingerprint(profiles["profiles"][request["profile"]]),
                    "versions": versions}
    lock_path = project / LOCK_FILES[0]
    document = read_json(lock_path)
    document.update(schema=2, profile=request["profile"], versions=request["versions"], resolved=profile_lock)
    lock_path.write_text(json.dumps(document, indent=2) + "\n", encoding="utf-8")
    checksum = fetch(f"https://services.gradle.org/distributions/gradle-{versions['gradle']}-bin.zip.sha256").decode().strip()
    if not re.fullmatch(r"[a-f0-9]{64}", checksum):
        raise ValueError("Invalid official Gradle distribution checksum")
    (project / LOCK_FILES[1]).write_text(
        "distributionBase=GRADLE_USER_HOME\ndistributionPath=wrapper/dists\n"
        f"distributionUrl=https\\://services.gradle.org/distributions/gradle-{versions['gradle']}-bin.zip\n"
        f"distributionSha256Sum={checksum}\nzipStoreBase=GRADLE_USER_HOME\nzipStorePath=wrapper/dists\n", encoding="utf-8")
    for name in LOCK_FILES[2:]:
        (project / name).unlink(missing_ok=True)
    java = str(Path(os.environ["JAVA_HOME"]) / "bin" / ("java.exe" if os.name == "nt" else "java")) if os.environ.get("JAVA_HOME") else shutil.which("java")
    if not java:
        raise ValueError("A JDK is required; set JAVA_HOME")
    command = [java, "-classpath", str(project / "gradle/wrapper/gradle-wrapper.jar"), "org.gradle.wrapper.GradleWrapperMain", "--no-daemon", "--max-workers=3", f"-PwbNativeRoot={native_root}",
               ":app:assembleDebug", ":app:assembleRelease"]
    subprocess.run(command + ["--write-locks", "--write-verification-metadata", "sha256"], cwd=project, check=True)
    seed_aapt2_platforms(project)
    # Re-run with the actual strict lock and checksum enforcement before publishing.
    subprocess.run(command + ["--offline", "--dependency-verification", "strict"], cwd=project, check=True)
    profile_lock["validated_profile_sha256"] = profile_digest(profile_lock)
    profile_lock["validated_files"] = {name: digest(project / name) for name in LOCK_FILES[1:]}
    lock_path.write_text(json.dumps(document, indent=2) + "\n", encoding="utf-8")


def resolve(project, native_root, request, profiles, update=False):
    with project_lock(project):
        return resolve_locked(project, native_root, request, profiles, update)


def resolve_locked(project, native_root, request, profiles, update=False):
    if not update and (project / LOCK_FILES[0]).exists():
        existing = read_json(project / LOCK_FILES[0]).get("resolved", {})
        if existing.get("request_hash") == fingerprint(request) and existing.get("profile_hash") == fingerprint(profiles["profiles"].get(request["profile"])):
            check_lock(project, request, profiles)
            print("Android dependency profile: reusing validated lock (no network)")
            return
    versions = choose_versions(request, profiles)
    print("Validating Android dependency profile: " + json.dumps(versions, sort_keys=True), flush=True)
    with tempfile.TemporaryDirectory(prefix="aria-android-profile-") as temporary:
        candidate = Path(temporary) / "android"
        shutil.copytree(project, candidate, ignore=shutil.ignore_patterns("build", ".gradle", ".cxx", "local.properties"))
        validate_candidate(candidate, native_root, versions, request, profiles)
        # Publish the single request/resolution document last.
        names = (*LOCK_FILES[1:], LOCK_FILES[0])
        previous = {name: (project / name).read_bytes() if (project / name).exists() else None for name in names}
        pending_files = []
        try:
            for name in names:
                destination = project / name
                destination.parent.mkdir(parents=True, exist_ok=True)
                descriptor, temporary_name = tempfile.mkstemp(prefix="." + destination.name + ".", dir=destination.parent)
                os.close(descriptor)
                pending = Path(temporary_name)
                pending_files.append(pending)
                shutil.copyfile(candidate / name, pending)
                pending.chmod(0o644)
                os.replace(pending, destination)
        except BaseException:
            for name, content in previous.items():
                destination = project / name
                if content is None:
                    destination.unlink(missing_ok=True)
                else:
                    destination.write_bytes(content)
            raise
        finally:
            for pending in pending_files:
                pending.unlink(missing_ok=True)
    print("Android dependency profile and Gradle locks updated; review all lock changes before committing")


def selected_toolchain(project, requested, profiles):
    definition = profiles["profiles"][requested["profile"]]
    versions = dict(definition)
    lock_path = project / LOCK_FILES[0]
    if lock_path.exists():
        locked = read_json(lock_path).get("resolved", {})
        if locked.get("request_hash") == fingerprint(requested) and locked.get("profile_hash") == fingerprint(definition):
            versions.update(check_lock(project, requested, profiles)["versions"])
    versions.update(requested["versions"])
    return versions


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("resolve", "update", "check", "sdk-packages", "toolchain"))
    parser.add_argument("--project", type=Path, default=ANDROID)
    parser.add_argument("--native-root", type=Path, default=ROOT)
    parser.add_argument("--profile")
    parser.add_argument("--field", choices=("ndk", "cmake", "java"))
    parser.add_argument("--version", action="append", default=[], metavar="NAME=VERSION")
    args = parser.parse_args(argv)
    overrides = {}
    for value in args.version:
        name, separator, version = value.partition("=")
        if not separator:
            parser.error("--version expects NAME=VERSION")
        if name in overrides:
            parser.error(f"Duplicate --version request: {name}")
        overrides[name] = version
    project = args.project.resolve()
    profiles = read_json(project / "android-dependencies.json")
    requested = requests(read_json(project / "android-dependencies.json"), args.profile, overrides, os.environ)
    if args.command == "toolchain":
        if not args.field:
            parser.error("toolchain requires --field")
        print(selected_toolchain(project, requested, profiles)[args.field])
    elif args.command in ("check", "sdk-packages"):
        lock = check_lock(project, requested, profiles)
        if args.command == "sdk-packages":
            versions = lock["versions"]
            for package in (f"platforms;{platform_package(versions['compile_sdk'])}", f"build-tools;{versions['build_tools']}",
                            f"ndk;{versions['ndk']}", f"cmake;{versions['cmake']}", "platform-tools"):
                print(package)
        else:
            print("Android profile and all validated lock files match")
    else:
        resolve(project, args.native_root.resolve(), requested, profiles, args.command == "update")


if __name__ == "__main__":
    try:
        main()
    except (ValueError, OSError, KeyError, subprocess.CalledProcessError) as error:
        print(f"Android dependency error: {error}", file=sys.stderr)
        raise SystemExit(1)
