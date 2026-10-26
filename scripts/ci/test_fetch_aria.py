"""Local-only safety regressions for the shared downstream Aria fetcher."""
import importlib.util
import json
import shutil
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

TEMPLATE = Path(__file__).with_name("fetch_aria.py")


def git(*args):
    result = subprocess.run(['git', *map(str, args)], capture_output=True, text=True)
    if result.returncode:
        raise AssertionError(f'git {args} failed: {result.stderr}')
    return result.stdout.strip()


class FetchTemplateTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='aria-fetch-fixture-')
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name).resolve()
        self.source = self.root / 'source'
        git('init', '-b', 'main', self.source)
        git('-C', self.source, 'config', 'user.email', 'test@example.invalid')
        git('-C', self.source, 'config', 'user.name', 'Aria fetch regression')
        (self.source / 'framework.txt').write_text('first\n')
        git('-C', self.source, 'add', 'framework.txt')
        git('-C', self.source, 'commit', '-m', 'first')
        self.first = git('-C', self.source, 'rev-parse', 'HEAD')
        (self.source / 'framework.txt').write_text('second\n')
        git('-C', self.source, 'commit', '-am', 'second')
        self.second = git('-C', self.source, 'rev-parse', 'HEAD')
        self.downstream = self.root / 'downstream'
        script = self.downstream / 'tools' / 'ci' / 'fetch_aria.py'
        self.script = script
        script.parent.mkdir(parents=True)
        script.write_text(TEMPLATE.read_text())
        shutil.copyfile(TEMPLATE.with_name("dependencies.py"), script.with_name("dependencies.py"))
        spec = importlib.util.spec_from_file_location('aria_fetch_fixture', script)
        self.module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(self.module)
        self.select(self.second)
        self.workspace = self.root / "shared-workspace"
        self.workspace.mkdir()
        environment = mock.patch.dict(os.environ, {"FORGEDEPS_HOME": str(self.workspace)})
        environment.start()
        self.addCleanup(environment.stop)
        self.dest = self.workspace / "sources/aria"
        self.module.DEST = self.dest
        self.dest.parent.mkdir(parents=True)

    def select(self, revision, version="1.0.0"):
        source = {"provider": "github", "repo": "dqsjqian/Aria", "artifact": "git", "tag_prefix": "v"}
        self.lock = self.downstream / "dependencies.json"
        import dependencies
        self.lock.write_text(json.dumps({"schema": 2, "dependencies": {"aria": {**source, "resolved": {
            "request_hash": dependencies.request_hash(source), "requested": "latest", "version": version,
            "tag": "v" + version, "revision": revision, "url": "https://github.com/dqsjqian/Aria.git", "sha256": ""
        }}}}))

    def checkout_first(self):
        git('clone', '--no-hardlinks', self.source, self.dest)
        git('-C', self.dest, 'checkout', '--detach', self.first)
        (self.dest / '.pinned-aria-sha').write_text(self.first + '\n')

    def run_fetch(self):
        return self.module.main(['--source', str(self.source)])

    def assert_no_transaction_artifacts(self):
        self.assertFalse((self.dest.parent / '.aria-fetch.lock').exists())
        self.assertEqual(list(self.dest.parent.glob('.aria-fetch-*')), [])
        self.assertEqual(list(self.dest.parent.glob('.bootstrap-*')), [])

    def test_environment_selects_local_source(self):
        with mock.patch.dict(os.environ, {'ARIA_SOURCE': str(self.source)}):
            self.assertEqual(self.module.main([]), 0)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        self.assert_no_transaction_artifacts()

    def test_explicit_source_overrides_environment(self):
        with mock.patch.dict(os.environ, {'ARIA_SOURCE': str(self.root / 'missing')}):
            self.assertEqual(self.run_fetch(), 0)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        self.assert_no_transaction_artifacts()

    def test_forged_marker_cannot_bypass_head_validation(self):
        self.checkout_first()
        (self.dest / '.pinned-aria-sha').write_text(self.second)
        # An altered marker is user data, not permission to replace the tree.
        with self.assertRaisesRegex(ValueError, 'local edits'):
            self.run_fetch()
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'first\n')
        self.assertEqual((self.dest / '.pinned-aria-sha').read_text(), self.second)
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()

    def test_dirty_tracked_dependency_is_not_replaced(self):
        self.checkout_first()
        (self.dest / 'framework.txt').write_text('local work\n')
        with self.assertRaisesRegex(ValueError, 'local edits'):
            self.run_fetch()
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'local work\n')
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()

    def test_untracked_dependency_work_is_not_replaced(self):
        self.checkout_first()
        (self.dest / 'new-source.cpp').write_text('// unfinished work\n')
        with self.assertRaisesRegex(ValueError, 'local edits'):
            self.run_fetch()
        self.assertTrue((self.dest / 'new-source.cpp').exists())
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assert_no_transaction_artifacts()

    def test_fetch_failure_preserves_old_checkout(self):
        self.checkout_first()
        empty = self.root / 'empty-mirror'
        git('init', '--bare', empty)
        with self.assertRaisesRegex(ValueError, 'Git operation failed'):
            self.module.main(['--source', str(empty)])
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'first\n')
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()

    def test_version_update_retains_complete_old_checkout(self):
        self.checkout_first()
        (self.dest / '.git' / 'info' / 'exclude').write_text('local-build/\n')
        (self.dest / 'local-build').mkdir()
        (self.dest / 'local-build' / 'artifact').write_text('preserve ignored artifacts')
        self.assertEqual(self.run_fetch(), 0)
        backups = list(self.dest.parent.glob('aria-backup-*'))
        self.assertEqual(len(backups), 1)
        self.assertEqual(git('-C', backups[0], 'rev-parse', 'HEAD'), self.first)
        self.assertEqual((backups[0] / 'local-build' / 'artifact').read_text(), 'preserve ignored artifacts')
        self.assertEqual((backups[0] / '.pinned-aria-sha').read_text().strip(), self.first)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        self.assertFalse((self.dest / '.pinned-aria-sha').exists())
        self.assert_no_transaction_artifacts()

    def test_non_git_directory_is_protected(self):
        self.dest.mkdir()
        (self.dest / 'valuable.txt').write_text('keep me')
        with self.assertRaisesRegex(ValueError, 'non-Git directory'):
            self.run_fetch()
        self.assertEqual((self.dest / 'valuable.txt').read_text(), 'keep me')
        self.assert_no_transaction_artifacts()

    def test_linked_worktree_is_not_relocated_into_an_invalid_backup(self):
        git('-C', self.source, 'worktree', 'add', '--detach', self.dest, self.first)
        (self.dest / '.pinned-aria-sha').write_text(self.first)
        with self.assertRaisesRegex(ValueError, 'linked worktree/submodule'):
            self.run_fetch()
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()

    def test_matching_head_reuses_legacy_marker_offline_without_fetching(self):
        self.checkout_first()
        self.select(self.first)
        self.assertEqual(self.module.main(['--source', str(self.root / 'missing'), '--offline']), 0)
        self.assertEqual((self.dest / '.pinned-aria-sha').read_text().strip(), self.first)
        self.assert_no_transaction_artifacts()

    def test_matching_head_preserves_an_altered_marker(self):
        self.checkout_first()
        self.select(self.first)
        (self.dest / '.pinned-aria-sha').write_text('forged')
        with self.assertRaisesRegex(ValueError, 'local edits'):
            self.run_fetch()
        self.assertEqual((self.dest / '.pinned-aria-sha').read_text(), 'forged')
        self.assert_no_transaction_artifacts()

    def test_marker_symlink_does_not_overwrite_its_target(self):
        self.checkout_first()
        self.select(self.first)
        marker = self.dest / '.pinned-aria-sha'
        marker.unlink()
        valuable = self.root / 'valuable.txt'
        valuable.write_text('do not overwrite')
        try:
            marker.symlink_to(valuable)
        except OSError as error:
            self.skipTest(f"Symlink creation unavailable: {error}")
        with self.assertRaisesRegex(ValueError, 'local edits'):
            self.run_fetch()
        self.assertEqual(valuable.read_text(), 'do not overwrite')
        self.assertTrue(marker.is_symlink())
        self.assertEqual(list(self.dest.glob('.aria-pin-*')), [])
        self.assert_no_transaction_artifacts()

    def test_failed_install_restores_original_checkout(self):
        self.checkout_first()
        original_rename = Path.rename
        def fail_staged_install(path, target):
            if path.parent.name.startswith('.bootstrap-') and Path(target) == self.dest:
                raise OSError('simulated install failure')
            return original_rename(path, target)
        with mock.patch.object(Path, 'rename', fail_staged_install):
            with self.assertRaisesRegex(OSError, 'simulated install failure'):
                self.run_fetch()
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'first\n')
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()

    def test_lock_write_failure_rolls_back_checkout_and_lock(self):
        self.checkout_first()
        original_lock = self.lock.read_bytes()
        shared = self.module.source_cache.deps
        original_write = shared.atomic_json
        installed = []
        def fail_lock_publication(path, value):
            if Path(path).resolve() == self.lock.resolve():
                installed.append(git('-C', self.dest, 'rev-parse', 'HEAD'))
                raise OSError("lock write failure")
            return original_write(path, value)
        with mock.patch.object(shared, "atomic_json", side_effect=fail_lock_publication):
            with self.assertRaisesRegex(OSError, "lock write failure"):
                self.run_fetch()
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual(self.lock.read_bytes(), original_lock)
        self.assertEqual(installed, [self.second])
        self.assert_no_transaction_artifacts()

    def test_explicit_version_reuses_matching_lock_offline(self):
        self.select(self.second, "2.0.0")
        self.assertEqual(self.module.main(['--source', str(self.source), '--version', '2.0.0', '--offline']), 0)
        selected = json.loads(self.lock.read_text())["dependencies"]["aria"]["resolved"]
        self.assertEqual(selected["requested"], "2.0.0")
        # A later ordinary build must keep this selection, without a new query.
        self.assertEqual(self.module.main(['--offline']), 0)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)

    def test_environment_version_and_explicit_override(self):
        self.select(self.second, "2.0.0")
        with mock.patch.dict(os.environ, {'ARIA_DEP_ARIA_VERSION': '2.0.0'}):
            self.assertEqual(self.module.main(['--source', str(self.source), '--offline']), 0)
        selected = json.loads(self.lock.read_text())["dependencies"]["aria"]["resolved"]
        self.assertEqual(selected["requested"], "2.0.0")
        with mock.patch.dict(os.environ, {'ARIA_DEP_ARIA_VERSION': '99.0.0'}):
            self.assertEqual(self.module.main(['--version', '2.0.0', '--offline']), 0)

    def test_default_fetch_registers_shared_source_for_sdk_and_offline_reuse(self):
        from forgedeps import deps_sources
        self.module.DEST = None
        def clone(destination, url, revision):
            git('clone', '--no-hardlinks', self.source, destination)
            git('-C', destination, 'checkout', '--detach', revision)
        with mock.patch.object(deps_sources, 'clone', side_effect=clone) as fetch:
            self.assertEqual(self.module.main([]), 0)
            self.assertEqual(self.module.main(['--offline']), 0)
            fetch.assert_called_once()
        receipt = self.workspace / 'state/sources/aria.json'
        self.assertTrue(receipt.is_file())
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        with self.assertRaisesRegex(ValueError, '--destination'):
            self.module.main(['--source', str(self.source)])

    def test_cli_alternate_lock_mirror_offline_reuse_and_dirty_failure(self):
        alternate = self.downstream / 'custom-lock.json'
        self.lock.rename(alternate)
        command = [sys.executable, str(self.script), '--file', str(alternate), '--offline', '--destination', str(self.dest)]
        first = subprocess.run(command + ['--source', str(self.source)], capture_output=True, text=True)
        self.assertEqual(first.returncode, 0, first.stderr)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        # Remove the source so only the already verified checkout can satisfy this.
        self.source.rename(self.root / 'unavailable-mirror')
        again = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(again.returncode, 0, again.stderr)
        before = alternate.read_bytes()
        (self.dest / 'framework.txt').write_text('local CLI work\n')
        dirty = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(dirty.returncode, 1)
        self.assertIn('local edits', dirty.stderr)
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'local CLI work\n')
        self.assertEqual(alternate.read_bytes(), before)
        self.assertFalse(self.lock.exists())
        self.assert_no_transaction_artifacts()

    def test_unresolved_version_offline_preserves_both_checkout_and_lock(self):
        self.checkout_first()
        before = self.lock.read_bytes()
        with self.assertRaisesRegex(ValueError, 'matching resolved'):
            self.module.main(['--version', '99.0.0', '--offline'])
        self.assertEqual(self.lock.read_bytes(), before)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assert_no_transaction_artifacts()

    def test_update_fetch_failure_keeps_previous_lock(self):
        self.checkout_first()
        before = self.lock.read_bytes()
        real_resolve = self.module.source_cache.deps.resolve
        def newer_resolution(manifest, lock, **kwargs):
            kwargs['update'] = False
            selection = real_resolve(manifest, lock, **kwargs)
            selection['dependencies']['aria']['revision'] = 'f' * 40
            selection['dependencies']['aria']['version'] = '9.0.0'
            return selection
        with mock.patch.object(self.module.source_cache.deps, 'resolve', side_effect=newer_resolution):
            with self.assertRaisesRegex(ValueError, 'failed'):
                self.module.main(['--source', str(self.source), '--update'])
        self.assertEqual(self.lock.read_bytes(), before)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assert_no_transaction_artifacts()


if __name__ == '__main__':
    unittest.main(verbosity=2)
