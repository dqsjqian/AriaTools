#!/usr/bin/env python3
"""Package selected native license texts and runtime JAR/AAR notices as APK assets.

This copies notices, not legal approval. Dependency updates still require review.
No dependency version is selected or changed here.
"""
from __future__ import annotations

import argparse
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import sys
import zipfile


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def stage(root, ndk, output, artifacts, json_license=None, runtime_components=()):
    native = root / "build/platforms/android"
    aria = Path((native / "aria-source.txt").read_text(encoding="utf-8").strip())
    headers = native / "include/nlohmann"
    selected_headers = Path((native / "include/nlohmann-source.txt").read_text(encoding="utf-8").strip())
    json_license = Path(json_license) if json_license else selected_headers.parent.parent / "LICENSE.MIT"
    host = "windows-x86_64" if os.name == "nt" else "darwin-x86_64" if sys.platform == "darwin" else "linux-x86_64"
    llvm = ndk / "toolchains/llvm/prebuilt" / host
    inputs = {
        "AriaTools-LICENSE.txt": root / "LICENSE",
        "Aria-LICENSE.txt": aria / "LICENSE",
        "nlohmann-json-LICENSE.txt": json_license,
        "Apache-2.0.txt": root / "licenses/Apache-2.0.txt",
        "NDK-LLVM-NOTICE.txt": llvm / "NOTICE",
        "NDK-sysroot-NOTICE.txt": llvm / "sysroot/NOTICE",
        "THIRD_PARTY_NOTICES.md": root / "THIRD_PARTY_NOTICES.md",
    }
    # Read every required source before replacing this generated output tree.
    texts = {name: path.read_bytes() for name, path in inputs.items()}
    attributions = set()
    versions = {}
    for header in sorted(headers.rglob("*.hpp")):
        for line in header.read_text(encoding="utf-8").splitlines():
            if "SPDX-FileCopyrightText:" in line or "SPDX-License-Identifier:" in line:
                attributions.add(line.strip())
            match = re.match(r"\s*#define NLOHMANN_JSON_VERSION_(MAJOR|MINOR|PATCH)\s+(\d+)", line)
            if match:
                versions[match[1]] = match[2]
    if not attributions:
        raise ValueError("The selected JSON headers have no SPDX attributions to package")
    texts["nlohmann-json-ATTRIBUTIONS.txt"] = ("\n".join(sorted(attributions)) + "\n").encode("utf-8")
    properties = (ndk / "source.properties").read_text(encoding="utf-8")
    ndk_version = re.search(r"^Pkg.Revision\s*=\s*(\S+)", properties, re.MULTILINE)
    if not ndk_version:
        raise ValueError("Selected NDK has no Pkg.Revision")
    # Gradle supplies its resolved graph, including when an update has not yet
    # published a new lockfile. Never label new artifacts using an old source lock.
    runtime = {}
    for entry in runtime_components:
        coordinate, configuration = entry.rsplit("=", 1)
        runtime.setdefault(coordinate, set()).add(configuration)
    if output.exists():
        shutil.rmtree(output)
    destination = output / "licenses"
    destination.mkdir(parents=True)
    for name, data in texts.items():
        (destination / name).write_bytes(data)
    records = []
    for artifact in sorted(set(artifacts)):
        data = artifact.read_bytes()
        digest = sha256(data)
        record = {"artifact": artifact.name, "sha256": digest, "notices": []}
        def collect(archive, prefix=""):
            for entry in archive.namelist():
                if entry.endswith("/"):
                    continue
                name = prefix + entry
                if re.search(r"(^|/)(LICENSE|LICENCE|NOTICE|COPYING)([._-]|$)", entry, re.IGNORECASE):
                    basename = re.sub(r"[^A-Za-z0-9._-]", "_", Path(entry).name)
                    relative = Path("maven") / digest[:16] / (sha256(name.encode())[:12] + "-" + basename)
                    target = destination / relative
                    target.parent.mkdir(parents=True, exist_ok=True)
                    content = archive.read(entry)
                    target.write_bytes(content)
                    record["notices"].append({"original_entry": name, "asset": relative.as_posix(), "sha256": sha256(content)})
                if entry == "classes.jar" and not prefix:
                    with zipfile.ZipFile(io.BytesIO(archive.read(entry))) as nested:
                        collect(nested, "classes.jar!/")
        if zipfile.is_zipfile(io.BytesIO(data)):
            with zipfile.ZipFile(io.BytesIO(data)) as archive:
                collect(archive)
        records.append(record)
    manifest = {
        "purpose": "Actual build inputs and copied license texts; not automatic legal clearance.",
        "review": "Dependency updates require reviewing upstream terms and notices. See THIRD_PARTY_NOTICES.md for the reviewed scope.",
        "ndk_version": ndk_version.group(1),
        "json_version": ".".join(versions.get(part, "unknown") for part in ("MAJOR", "MINOR", "PATCH")),
        "native_texts": {name: sha256(data) for name, data in texts.items()},
        "runtime_components_including_platform_metadata": [
            {"coordinate": coordinate, "configurations": sorted(configurations)}
            for coordinate, configurations in sorted(runtime.items())
        ],
        "runtime_artifacts": records,
    }
    (destination / "BUILD-LICENSES.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"Packaged selected license texts and notices from {len(records)} runtime artifacts; dependency updates require license review.")
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--native-root", required=True, type=Path)
    parser.add_argument("--ndk", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--artifact", action="append", default=[], type=Path)
    parser.add_argument("--runtime-component", action="append", default=[])
    parser.add_argument("--json-license", default=os.environ.get("ARIA_JSON_LICENSE_FILE"))
    args = parser.parse_args()
    stage(args.native_root.resolve(), args.ndk.resolve(), args.output.resolve(), args.artifact, args.json_license, args.runtime_component)


if __name__ == "__main__":
    main()
