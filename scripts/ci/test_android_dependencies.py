#!/usr/bin/env python3
"""Offline regressions for Android profile selection and lock replacement."""
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location("android_dependencies", Path(__file__).with_name("android_dependencies.py"))
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class AndroidDependencyTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.project = Path(self.temp.name)
        self.profiles = {"profiles": {"test": {"agp_series": "9.4.", "gradle": "9.6.0", "kotlin": "2.4.20"}}}
        self.request = {"profile": "test", "versions": {}}
        self.metadata = b"<metadata><versioning><versions><version>9.4.0</version><version>9.4.1</version><version>9.4.2-rc01</version><version>9.5.0</version></versions></versioning></metadata>"

    def lock(self):
        files = {}
        for name in module.LOCK_FILES[1:]:
            p = self.project / name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text("original")
            files[name] = module.digest(p)
        versions = {key: "1.0.0" for key in (*module.ARTIFACTS, "kotlin", "gradle", "build_tools", "ndk", "cmake")}
        versions.update(compile_sdk="37", java="17")
        resolved = {"versions": versions, "request_hash": module.fingerprint(self.request),
                    "profile_hash": module.fingerprint(self.profiles["profiles"]["test"]), "validated_files": files}
        resolved["validated_profile_sha256"] = module.profile_digest(resolved)
        data = {"schema": 2, **self.request, **self.profiles, "resolved": resolved}
        (self.project / module.LOCK_FILES[0]).write_text(json.dumps(data))

    def test_stable_semantic_sort_excludes_previews(self):
        data = self.metadata.replace(b"9.5.0", b"9.10.0")
        self.assertEqual(module.stable_versions(data), ["9.4.0", "9.4.1", "9.10.0"])

    def test_explicit_cli_over_environment_over_manifest(self):
        manifest = {"schema": 2, "profile": "test", "versions": {"activity": "1.1.0"}}
        result = module.requests(manifest, None, {"activity": "1.3.0"}, {"ARIA_DEP_ANDROID_ACTIVITY_VERSION": "1.2.0"})
        self.assertEqual(result["versions"]["activity"], "1.3.0")

    def test_unknown_or_injected_version_rejected(self):
        for explicit in [{"bad": "1.0"}, {"activity": "1.0; command"}, {"activity": "1.0-beta"}]:
            with self.assertRaises(ValueError):
                module.requests({"schema": 2, "profile": "test"}, None, explicit, {})

    def test_profile_prevents_independent_agp_major_upgrade(self):
        selected = module.choose_versions(self.request, self.profiles, lambda _: self.metadata)
        self.assertEqual(selected["agp"], "9.4.1")
        self.assertEqual(selected["activity"], "9.5.0")
        self.assertEqual(selected["gradle"], "9.6.0")

    def test_explicit_library_release_wins(self):
        self.request["versions"]["activity"] = "9.4.0"
        self.assertEqual(module.choose_versions(self.request, self.profiles, lambda _: self.metadata)["activity"], "9.4.0")

    def test_unsupported_toolchain_override_rejected(self):
        for values in ({"agp": "9.5.0"}, {"gradle": "8.7"}):
            self.request["versions"] = values
            with self.assertRaises(ValueError):
                module.choose_versions(self.request, self.profiles, lambda _: self.metadata)

    def test_existing_validated_lock_reused_without_network(self):
        self.lock()
        with patch.object(module, "choose_versions", side_effect=AssertionError("network")):
            module.resolve(self.project, self.project, self.request, self.profiles)

    def test_tampered_lock_is_not_silently_refreshed(self):
        self.lock()
        (self.project / module.LOCK_FILES[-1]).write_text("tampered")
        with patch.object(module, "choose_versions", side_effect=AssertionError("network")):
            with self.assertRaisesRegex(ValueError, "changed"):
                module.resolve(self.project, self.project, self.request, self.profiles)

    def test_failed_candidate_preserves_every_source_lock(self):
        self.lock()
        before = {n: (self.project / n).read_bytes() for n in module.LOCK_FILES}
        with patch.object(module, "choose_versions", return_value={}), patch.object(module, "validate_candidate", side_effect=ValueError("incompatible")):
            with self.assertRaisesRegex(ValueError, "incompatible"):
                module.resolve(self.project, self.project, self.request, self.profiles, update=True)
        self.assertEqual(before, {n: (self.project / n).read_bytes() for n in module.LOCK_FILES})

    def test_interrupted_publication_rolls_back_all_files(self):
        self.lock()
        before = {n: (self.project / n).read_bytes() for n in module.LOCK_FILES}
        def validated(candidate, *args):
            for name in module.LOCK_FILES:
                (candidate / name).write_text("new")
        original_replace = module.os.replace
        count = 0
        def interrupted(source, destination):
            nonlocal count
            count += 1
            if count == 2:
                raise KeyboardInterrupt()
            original_replace(source, destination)
        with patch.object(module, "choose_versions", return_value={}), patch.object(module, "validate_candidate", side_effect=validated), patch.object(module.os, "replace", side_effect=interrupted):
            with self.assertRaises(KeyboardInterrupt):
                module.resolve(self.project, self.project, self.request, self.profiles, update=True)
        self.assertEqual(before, {n: (self.project / n).read_bytes() for n in module.LOCK_FILES})
        self.assertEqual((self.project / "android-dependencies.json").read_bytes(), before[module.LOCK_FILES[0]])

    def test_successful_update_preserves_single_document_and_profile_definitions(self):
        self.lock()
        def validated(candidate, *args):
            path = candidate / module.LOCK_FILES[0]
            document = json.loads(path.read_text())
            resolved = document["resolved"]
            resolved["versions"]["activity"] = "9.4.1"
            resolved["validated_profile_sha256"] = module.profile_digest(resolved)
            path.write_text(json.dumps(document))
        with patch.object(module, "choose_versions", return_value={}), patch.object(module, "validate_candidate", side_effect=validated):
            module.resolve(self.project, self.project, self.request, self.profiles, update=True)
        document = json.loads((self.project / module.LOCK_FILES[0]).read_text())
        self.assertEqual(document["profiles"], self.profiles["profiles"])
        self.assertEqual(document["resolved"]["versions"]["activity"], "9.4.1")
        self.assertNotIn("request", document["resolved"])
        self.assertNotIn("profile_definition", document["resolved"])
        self.assertEqual(list(self.project.glob("**/*.json")), [self.project / "android-dependencies.json"])
        module.check_lock(self.project, self.request, self.profiles)

    def test_request_edit_invalidates_record_without_duplicate_source_copy(self):
        self.lock()
        changed = {"profile": "test", "versions": {"activity": "1.2.0"}}
        with self.assertRaisesRegex(ValueError, "requests changed"):
            module.check_lock(self.project, changed, self.profiles)

    def test_changed_profile_does_not_reuse_old_toolchain(self):
        self.lock()
        self.profiles["profiles"]["test"]["ndk"] = "30.0.0"
        self.assertEqual(module.selected_toolchain(self.project, self.request, self.profiles)["ndk"], "30.0.0")

    def test_matching_damaged_profile_rejected_before_native_build(self):
        self.lock()
        (self.project / module.LOCK_FILES[-1]).write_text("damaged")
        with self.assertRaisesRegex(ValueError, "changed"):
            module.selected_toolchain(self.project, self.request, self.profiles)

    def test_platform_package_preserves_older_integer_sdk_names(self):
        self.assertEqual(module.platform_package("34"), "android-34")
        self.assertEqual(module.platform_package("37"), "android-37.0")

    def test_modified_selected_versions_rejected(self):
        self.lock()
        p = self.project / module.LOCK_FILES[0]
        data = json.loads(p.read_text()); data["resolved"]["versions"]["activity"] = "99.0.0"; p.write_text(json.dumps(data))
        with self.assertRaisesRegex(ValueError, "changed after validation"):
            module.check_lock(self.project, self.request, self.profiles)

    def test_missing_selected_versions_rejected(self):
        self.lock()
        p = self.project / module.LOCK_FILES[0]
        data = json.loads(p.read_text()); data["resolved"].pop("versions"); p.write_text(json.dumps(data))
        with self.assertRaisesRegex(ValueError, "complete selected versions"):
            module.check_lock(self.project, self.request, self.profiles)

    def test_concurrent_project_update_rejected(self):
        with module.project_lock(self.project):
            with self.assertRaisesRegex(ValueError, "already running"):
                with module.project_lock(self.project):
                    self.fail("Concurrent lock unexpectedly acquired")

    def test_verification_uses_cold_cache_and_replays_that_same_cache(self):
        homes = []
        def run(command, **kwargs):
            home = Path(kwargs["env"]["GRADLE_USER_HOME"])
            self.assertNotEqual(str(home), "existing-warm-cache")
            self.assertEqual(command[command.index("--gradle-user-home") + 1], str(home))
            self.assertTrue(kwargs["check"])
            if not homes:
                self.assertEqual(list(home.iterdir()), [])
                self.assertIn("--write-verification-metadata", command)
                (home / "downloaded-artifact").write_text("verified")
            else:
                self.assertEqual(home, homes[0])
                self.assertTrue((home / "downloaded-artifact").is_file())
                self.assertIn("--offline", command)
                self.assertEqual(command[-2:], ["--dependency-verification", "strict"])
            homes.append(home)
        with patch.dict(module.os.environ, {"GRADLE_USER_HOME": "existing-warm-cache", "ANDROID_SDK_ROOT": "selected-sdk"}), patch.object(module.subprocess, "run", side_effect=run), patch.object(module, "seed_aapt2_platforms"):
            module.validate_gradle_project(self.project, self.project, "java")
        self.assertEqual(len(homes), 2)
        self.assertFalse(homes[0].exists())

    def test_selected_sdk_precedence_is_applied_only_to_gradle_children(self):
        with patch.dict(module.os.environ, {"ANDROID_HOME": "old-sdk", "ANDROID_SDK_ROOT": "selected-sdk"}):
            parent_environment = dict(module.os.environ)
            with patch.object(module.subprocess, "run") as run, patch.object(module, "seed_aapt2_platforms"):
                module.validate_gradle_project(self.project, self.project, "java")
            self.assertEqual(run.call_count, 2)
            for call in run.call_args_list:
                environment = call.kwargs["env"]
                self.assertEqual(environment["ANDROID_HOME"], "selected-sdk")
                self.assertEqual(environment["ANDROID_SDK_ROOT"], "selected-sdk")
            self.assertEqual(dict(module.os.environ), parent_environment)

    def test_missing_validation_record_rejected(self):
        self.lock()
        p = self.project / module.LOCK_FILES[0]
        data = json.loads(p.read_text()); data["resolved"].pop("validated_files"); p.write_text(json.dumps(data))
        with self.assertRaisesRegex(ValueError, "complete Gradle"):
            module.check_lock(self.project, self.request, self.profiles)


if __name__ == "__main__":
    unittest.main()
