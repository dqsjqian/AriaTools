#!/usr/bin/env python3
"""Verify selected licenses, UTF-8 attribution, and safe APK notice paths."""
import importlib.util
import io
import json
from pathlib import Path
import tempfile
import unittest
import zipfile

spec = importlib.util.spec_from_file_location("stage_android_notices", Path(__file__).with_name("stage_android_notices.py"))
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class StageAndroidNoticesTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / "LICENSE").write_bytes(b"actual AriaTools license")
        native = self.root / "build/platforms/android"
        self.headers = native / "include/nlohmann"
        self.headers.mkdir(parents=True)
        selected = self.root / "selected-json/include/nlohmann"
        selected.mkdir(parents=True)
        (selected.parent.parent / "LICENSE.MIT").write_bytes(b"actual selected JSON license")
        (native / "include/nlohmann-source.txt").write_text(str(selected), encoding="utf-8")
        aria = self.root / "selected-aria"
        aria.mkdir()
        (aria / "LICENSE").write_bytes(b"actual selected Aria license")
        (native / "aria-source.txt").write_text(str(aria), encoding="utf-8")
        (self.headers / "json.hpp").write_text("// SPDX-FileCopyrightText: 2008 - 2009 Björn Hoehrmann\n// SPDX-License-Identifier: MIT\n#define NLOHMANN_JSON_VERSION_MAJOR 3\n#define NLOHMANN_JSON_VERSION_MINOR 12\n#define NLOHMANN_JSON_VERSION_PATCH 0\n", encoding="utf-8")
        (self.root / "licenses").mkdir()
        (self.root / "licenses/Apache-2.0.txt").write_bytes(b"original Apache text")
        (self.root / "THIRD_PARTY_NOTICES.md").write_text("review scope")
        self.ndk = self.root / "ndk"
        host = "windows-x86_64" if module.os.name == "nt" else "darwin-x86_64" if module.sys.platform == "darwin" else "linux-x86_64"
        llvm = self.ndk / "toolchains/llvm/prebuilt" / host
        (llvm / "sysroot").mkdir(parents=True)
        (llvm / "NOTICE").write_bytes(b"selected LLVM notice")
        (llvm / "sysroot/NOTICE").write_bytes(b"selected sysroot notice")
        (self.ndk / "source.properties").write_text("Pkg.Revision = 29.0.14206865\n")
        self.output = self.root / "generated"

    def test_selected_texts_all_headers_and_safe_nested_archive_entries(self):
        (self.headers / "other.hpp").write_text("// SPDX-FileCopyrightText: 2018 The Abseil Authors\n// SPDX-License-Identifier: MIT\n", encoding="utf-8")
        nested = io.BytesIO()
        with zipfile.ZipFile(nested, "w") as archive:
            archive.writestr("../../NOTICE.txt", b"nested upstream notice")
        artifact = self.root / "runtime.aar"
        with zipfile.ZipFile(artifact, "w") as archive:
            archive.writestr("META-INF/LICENSE.txt", b"archive license")
            archive.writestr("classes.jar", nested.getvalue())
        result = module.stage(self.root, self.ndk, self.output, [artifact], runtime_components=[
            "g:runtime:2=debugRuntimeClasspath", "g:runtime:2=releaseRuntimeClasspath"])
        assets = self.output / "licenses"
        self.assertEqual((assets / "AriaTools-LICENSE.txt").read_bytes(), b"actual AriaTools license")
        self.assertEqual((assets / "Aria-LICENSE.txt").read_bytes(), b"actual selected Aria license")
        self.assertEqual((assets / "nlohmann-json-LICENSE.txt").read_bytes(), b"actual selected JSON license")
        self.assertEqual((assets / "NDK-LLVM-NOTICE.txt").read_bytes(), b"selected LLVM notice")
        attributions = (assets / "nlohmann-json-ATTRIBUTIONS.txt").read_text(encoding="utf-8")
        self.assertIn("Björn Hoehrmann", attributions)
        self.assertIn("The Abseil Authors", attributions)
        self.assertEqual(attributions.count("SPDX-License-Identifier: MIT"), 1)
        self.assertEqual(result["json_version"], "3.12.0")
        self.assertEqual(len(result["runtime_components_including_platform_metadata"]), 1)
        self.assertEqual(result["runtime_components_including_platform_metadata"][0], {
            "coordinate": "g:runtime:2", "configurations": ["debugRuntimeClasspath", "releaseRuntimeClasspath"]})
        notices = result["runtime_artifacts"][0]["notices"]
        self.assertEqual(len(notices), 2)
        for notice in notices:
            self.assertTrue((assets / notice["asset"]).resolve().is_relative_to(assets.resolve()))
        self.assertNotIn(str(self.root), json.dumps(result))

    def test_missing_required_source_preserves_existing_output(self):
        self.output.mkdir()
        sentinel = self.output / "existing"
        sentinel.write_text("old output")
        (self.root / "licenses/Apache-2.0.txt").unlink()
        with self.assertRaises(FileNotFoundError):
            module.stage(self.root, self.ndk, self.output, [])
        self.assertEqual(sentinel.read_text(), "old output")


if __name__ == "__main__":
    unittest.main()
