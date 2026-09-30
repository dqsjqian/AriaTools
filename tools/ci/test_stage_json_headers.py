#!/usr/bin/env python3
"""Regression checks for exporting the JSON target selected by CMake."""
import pathlib
import subprocess
import tempfile
import unittest


HELPER = pathlib.Path(__file__).resolve().parents[2] / "Workbench/cmake/StageJsonHeaders.cmake"


class StageJsonHeadersTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = pathlib.Path(self.temporary.name)
        self.manifest = self.root / "selected-includes.txt"
        self.destination = self.root / "export/include/nlohmann"

    def headers(self, include, content):
        header = include / "nlohmann/json.hpp"
        header.parent.mkdir(parents=True, exist_ok=True)
        header.write_text(content)
        return header

    def stage(self):
        return subprocess.run(
            ["cmake", f"-DWB_JSON_MANIFEST={self.manifest}",
             f"-DWB_JSON_DESTINATION={self.destination}", "-P", str(HELPER)],
            capture_output=True, text=True)

    def test_selected_target_wins_over_old_cache_and_replaces_old_files(self):
        self.headers(self.root / "cache/json-old/include", "old cached release")
        selected = self.root / "selected version with spaces/include"
        self.headers(selected, "selected release")
        self.destination.mkdir(parents=True)
        (self.destination / "removed-in-new-release.hpp").write_text("obsolete")
        self.manifest.write_text(f"{self.root / 'unrelated/include'}\n{selected}\n")
        result = self.stage()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual((self.destination / "json.hpp").read_text(), "selected release")
        self.assertEqual((self.destination.parent / "nlohmann-source.txt").read_text().strip(), str((selected / "nlohmann").resolve()))
        self.assertFalse((self.destination / "removed-in-new-release.hpp").exists())

    def test_invalid_selection_preserves_existing_export(self):
        self.headers(self.destination.parent, "existing export")
        self.manifest.write_text(str(self.root / "missing/include") + "\n")
        self.assertNotEqual(self.stage().returncode, 0)
        self.assertEqual((self.destination / "json.hpp").read_text(), "existing export")

    def test_source_can_already_be_the_destination(self):
        self.headers(self.destination.parent, "already exported")
        self.manifest.write_text(str(self.destination.parent) + "\n")
        result = self.stage()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual((self.destination / "json.hpp").read_text(), "already exported")


if __name__ == "__main__":
    unittest.main()
