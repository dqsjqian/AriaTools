"""Local-only safety regressions for the shared downstream Aria fetcher."""
import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest import mock

FETCHER = Path(__file__).with_name('fetch_aria.py')


def git(*args):
    result = subprocess.run(['git', *map(str, args)], capture_output=True, text=True)
    if result.returncode:
        raise AssertionError(f'git {args} failed: {result.stderr}')
    return result.stdout.strip()


class FetchAriaTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='aria-fetch-fixture-')
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
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
        script.parent.mkdir(parents=True)
        script.write_text(FETCHER.read_text())
        spec = importlib.util.spec_from_file_location('aria_fetch_fixture', script)
        self.module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(self.module)
        self.module.ARIA_SHA = self.second
        self.dest = self.module.DEST
        self.dest.parent.mkdir(parents=True)

    def checkout_first(self):
        git('clone', '--no-hardlinks', self.source, self.dest)
        git('-C', self.dest, 'checkout', '--detach', self.first)
        (self.dest / '.pinned-aria-sha').write_text(self.first + '\n')

    def run_fetch(self):
        return self.module.main(['--source', str(self.source)])

    def assert_no_transaction_artifacts(self):
        self.assertFalse((self.dest.parent / '.aria-fetch.lock').exists())
        self.assertEqual(list(self.dest.parent.glob('.aria-fetch-*')), [])

    def test_environment_source_supports_unpublished_local_commit(self):
        with mock.patch.dict(self.module.os.environ, {'ARIA_SOURCE': str(self.source)}):
            self.assertEqual(self.module.main([]), 0)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        self.assert_no_transaction_artifacts()

    def test_explicit_source_overrides_environment(self):
        with mock.patch.dict(self.module.os.environ, {'ARIA_SOURCE': str(self.root / 'missing')}):
            self.assertEqual(self.run_fetch(), 0)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        self.assert_no_transaction_artifacts()

    def test_forged_marker_cannot_bypass_head_validation(self):
        self.checkout_first()
        (self.dest / '.pinned-aria-sha').write_text(self.second)
        self.assertEqual(self.run_fetch(), 0)
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.second)
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'second\n')
        backups = list(self.dest.parent.glob('aria-backup-*'))
        self.assertEqual(len(backups), 1)
        self.assertEqual(git('-C', backups[0], 'rev-parse', 'HEAD'), self.first)
        self.assert_no_transaction_artifacts()

    def test_dirty_tracked_dependency_is_not_replaced(self):
        self.checkout_first()
        (self.dest / 'framework.txt').write_text('local work\n')
        with self.assertRaisesRegex(RuntimeError, 'local edits'):
            self.run_fetch()
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'local work\n')
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()

    def test_untracked_dependency_work_is_not_replaced(self):
        self.checkout_first()
        (self.dest / 'new-source.cpp').write_text('// unfinished work\n')
        with self.assertRaisesRegex(RuntimeError, 'local edits'):
            self.run_fetch()
        self.assertTrue((self.dest / 'new-source.cpp').exists())
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assert_no_transaction_artifacts()

    def test_fetch_failure_preserves_old_checkout(self):
        self.checkout_first()
        with self.assertRaisesRegex(RuntimeError, 'failed'):
            self.module.main(['--source', str(self.root / 'missing-local-repository')])
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
        self.assertEqual((self.dest / '.pinned-aria-sha').read_text().strip(), self.second)
        self.assert_no_transaction_artifacts()

    def test_non_git_directory_is_protected(self):
        self.dest.mkdir()
        (self.dest / 'valuable.txt').write_text('keep me')
        with self.assertRaisesRegex(RuntimeError, 'non-Git directory'):
            self.run_fetch()
        self.assertEqual((self.dest / 'valuable.txt').read_text(), 'keep me')
        self.assert_no_transaction_artifacts()

    def test_linked_worktree_is_not_relocated_into_an_invalid_backup(self):
        git('-C', self.source, 'worktree', 'add', '--detach', self.dest, self.first)
        (self.dest / '.pinned-aria-sha').write_text(self.first)
        with self.assertRaisesRegex(RuntimeError, 'linked worktree or submodule'):
            self.run_fetch()
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()

    def test_matching_head_repairs_marker_without_fetching(self):
        self.checkout_first()
        self.module.ARIA_SHA = self.first
        (self.dest / '.pinned-aria-sha').write_text('forged')
        self.assertEqual(self.module.main(['--source', str(self.root / 'missing')]), 0)
        self.assertEqual((self.dest / '.pinned-aria-sha').read_text().strip(), self.first)
        self.assert_no_transaction_artifacts()

    def test_marker_symlink_does_not_overwrite_its_target(self):
        self.checkout_first()
        self.module.ARIA_SHA = self.first
        marker = self.dest / '.pinned-aria-sha'
        marker.unlink()
        valuable = self.root / 'valuable.txt'
        valuable.write_text('do not overwrite')
        marker.symlink_to(valuable)
        self.assertEqual(self.run_fetch(), 0)
        self.assertEqual(valuable.read_text(), 'do not overwrite')
        self.assertFalse(marker.is_symlink())
        self.assertEqual(marker.read_text().strip(), self.first)
        self.assertEqual(list(self.dest.glob('.aria-pin-*')), [])
        self.assert_no_transaction_artifacts()

    def test_failed_install_restores_original_checkout(self):
        self.checkout_first()
        original_rename = Path.rename
        def fail_staged_install(path, target):
            if path.parent.name.startswith('.aria-fetch-') and Path(target) == self.dest:
                raise OSError('simulated install failure')
            return original_rename(path, target)
        with mock.patch.object(Path, 'rename', fail_staged_install):
            with self.assertRaisesRegex(OSError, 'simulated install failure'):
                self.run_fetch()
        self.assertEqual(git('-C', self.dest, 'rev-parse', 'HEAD'), self.first)
        self.assertEqual((self.dest / 'framework.txt').read_text(), 'first\n')
        self.assertEqual(list(self.dest.parent.glob('aria-backup-*')), [])
        self.assert_no_transaction_artifacts()


if __name__ == '__main__':
    unittest.main(verbosity=2)
