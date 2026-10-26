#!/usr/bin/env python3
"""Compatibility entry point for the shared ForgeDeps dependency resolver.

Keep the project's resolve/update commands and Python imports while the
resolution, validation and transactional writes live in forgedeps.
"""
from pathlib import Path
import sys

try:
    from forgedeps import deps_build as _shared
    from forgedeps.__main__ import main as _shared_main
except ModuleNotFoundError as error:
    if error.name != "forgedeps":
        raise
    raise SystemExit(
        f'ForgeDeps is missing. Run "{sys.executable}" -m pip install -r '
        f'"{Path(__file__).resolve().parents[2] / "requirements-build.txt"}"'
    ) from None

DependencyError = _shared.DependencyError
read_resolved = _shared.read_resolved
request_hash = _shared.request_hash
validate_record = _shared.validate_record


class Context(_shared.Context):
    def __init__(self, cache_dir, offline=False):
        super().__init__(cache_dir, offline, user_agent="aria-dependencies")


def resolve(file, effective_file=None, *, versions=None, only=None, update=False, context=None):
    return _shared.resolve(
        file, effective_file, versions=versions, only=only, update=update,
        context=context or Context(Path(file).parent / "build/deps/resolver"),
    )


def main(argv=None, *, command=None, file=None):
    arguments = list(sys.argv[1:] if argv is None else argv)
    if command is None:
        if not arguments or arguments[0] not in ("resolve", "update"):
            return _shared_main(arguments)
        command = arguments.pop(0)
    if file is not None:
        arguments = ["--file", str(file), *arguments]
    if command == "update":
        arguments = ["--update", *arguments]
    return _shared_main(["resolve", *arguments])


if __name__ == "__main__":
    raise SystemExit(main())
