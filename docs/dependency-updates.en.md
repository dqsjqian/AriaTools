# Updating dependencies

Run these commands from the **AriaTools repository root**. Use Python 3.14+; install `python -m pip install -r requirements-build.txt` with the build interpreter (preferably in a virtual environment). The repository scripts delegate resolution, verification and Git bootstrap to shared ForgeDeps; substitute `python3` when required on macOS/Linux. Online resolution can use an authenticated `gh` CLI, or the public GitHub API. Install the compiler, CMake and platform SDKs listed in the README separately.

The single `dependencies.json` declares sources and optional persistent `version` requirements. Each entry’s `resolved` object records the selected version, complete Git commit, and download SHA256. Commit this file when it changes. Normal builds require matching locks; an explicit resolve fills missing selections. Automatic latest selection excludes prereleases and development branches. Explicit versions may select prereleases or downstream patches that resolve to real version tags.

## Commands

```bash
python scripts/ci/update_dependencies.py --help
python scripts/ci/update_dependencies.py
python scripts/ci/update_dependencies.py --only mira
python scripts/ci/update_dependencies.py --only json --only mira
python scripts/ci/update_dependencies.py --version json=3.12.0 --version openssl=4.0.3
python scripts/ci/update_dependencies.py --only json --only mira --version json=3.12.0
```

The available, case-sensitive names are: `aria`, `doctest`, `json`, `mira`, `openssl`. Repeat `--only` for multiple selections and `--version` for different overrides. When using both, select every overridden name with `--only`. Unknown names, duplicate overrides and overrides outside the selection are errors.

To keep two dependencies fixed while updating the third, add `"version": "3.12.0"` to the existing JSON entry and `"version": "4.0.3"` to the existing OpenSSL entry; preserve all their source fields. Leave `mira` without a `version` field. A plain updater run then respects the two fixed versions and selects the latest stable release for each unpinned dependency. `--only mira` instead changes only Mira, leaving every other record untouched. No script edits are needed.

Precedence: this invocation's `--version` overrides the manifest; explicit manifest versions override defaults. Without an explicit request, ordinary resolution reuses the lock and deliberate updating discovers the latest stable version. A command-line selection remains locked when the manifest does not explicitly request a different version. If it does, the next resolution without that override restores the manifest request. Command-line overrides do not change persistent manifest requirements; record them there if they should constrain later updates. To unpin a library, remove its manifest `version` and deliberately update it.

## Shared SDK build workflow

Install the build tools and register one workspace shared by your projects:

```bash
python -m pip install -r requirements-build.txt
python -m forgedeps init /path/to/ForgeDepsWorkspace
python scripts/build.py --test
```

Builds use the recorded `dependencies.json` selection without selecting new releases. `FORGEDEPS_HOME` overrides the registered workspace. Sources live in `sources/`; Aria and third-party SDKs live in `build/<name>/<target>/`. Applications link those prefixes directly. Each target keeps one current artifact; changes to versions, compilers, ABI, source contents or recipes rebuild it transactionally and preserve the previous artifact on failure.

`--aria-root /path/to/Aria` (CMake `ARIA_DIR`, or Tutorial `ARIA_ROOT`) selects editable sources which are still content-hashed and built into the shared SDK. Qt, NDK and Apple SDKs are installed separately; actual toolchain and Qt contents participate in artifact identity. Use `scripts/ci/update_dependencies.py` to change versions, then review and commit the lock.

The Python entry holds reader leases through configure, build, tests and deployment. For manual commands use `python -m forgedeps.build_kit.session --configure -- cmake -S ... -B ...`, then `python -m forgedeps.build_kit.session --build-dir <build> -- <command>`. Offline builds require prepared sources/downloads and the toolchain; incompatible or modified artifacts fail verification.

`fetch_aria.py` is optional shared-source preparation. Local mirror development checkouts require `--source <repo> --destination <path>`. Application builds prepare their SDK automatically.
