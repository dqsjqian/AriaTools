#!/usr/bin/env python3
"""Fetch the locked Aria source through ForgeDeps, preserving local work.

--version overrides ARIA_DEP_ARIA_VERSION; --source overrides ARIA_SOURCE.
Local mirrors must contain the locked commit. Offline reuse and transactional
backup/rollback are implemented by the shared bootstrap_git API.
"""
import argparse
import os
from pathlib import Path
import sys

try:
    from forgedeps import source_cache
    from forgedeps.home import prepare_home
except ModuleNotFoundError as error:
    if error.name != "forgedeps":
        raise
    raise SystemExit(
        f'ForgeDeps is missing. Run "{sys.executable}" -m pip install -r '
        f'"{Path(__file__).resolve().parents[2] / "requirements-build.txt"}"'
    ) from None

ROOT = Path(__file__).resolve().parents[2]
DEST = None  # Optional embedding override; defaults to the managed workspace.


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--destination", type=Path, help="Explicit editable checkout destination")
    parser.add_argument("--source", default=os.environ.get("ARIA_SOURCE"),
                        help="Override the locked Git URL with a local repository or mirror")
    parser.add_argument("--version", default=os.environ.get("ARIA_DEP_ARIA_VERSION"),
                        help="Explicit version tag for this invocation")
    parser.add_argument("--file", type=Path, default=ROOT / "dependencies.json",
                        help="Dependency requirements and embedded resolved results")
    parser.add_argument("--update", action="store_true", help="Resolve the selection again")
    parser.add_argument("--offline", action="store_true", help="Require a recorded selection and local checkout/source")
    args = parser.parse_args(argv)
    workspace = prepare_home()
    destination = args.destination or DEST
    if destination is None:
        if args.source:
            raise ValueError("--source requires --destination for an explicit editable checkout")
        from forgedeps import deps_build
        from forgedeps.workspace_sources import prepare_source
        from forgerecipes import make_sdk_config
        config = make_sdk_config(shared=False)
        selection = deps_build.resolve(args.file, only=["aria"],
            versions={"aria": args.version} if args.version else None,
            update=args.update,
            context=deps_build.Context(workspace / "cache/resolver", args.offline))
        dependency = deps_build.locked_recipes(selection, config.recipes, config)[0]
        source = prepare_source(dependency, home=workspace, offline=args.offline)
        result = {"source_dir": str(source), "revision": dependency.revision}
    else:
        result = source_cache.bootstrap_git(
            args.file, "aria", destination, version=args.version, update=args.update,
            offline=args.offline, source=args.source, cache=workspace / "cache/resolver",
        )
    if result.get("backup"):
        print("Previous Aria checkout retained at " + result["backup"])
    print("Aria verified at %s -> %s" % (result["revision"][:12], result["source_dir"]))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError) as error:
        print("error: %s" % error, file=sys.stderr)
        raise SystemExit(1)
