# Updating dependencies

Run these commands from the **AriaTools repository root**. Use Python 3.10+ with its standard library; substitute `python3` when required on macOS/Linux. Online resolution can use an authenticated `gh` CLI, or the public GitHub API. Install the compiler, CMake and platform SDKs listed in the README separately.

The single `dependencies.json` declares sources and optional persistent `version` requirements. Each entry’s `resolved` object records the selected version, complete Git commit, and download SHA256. Commit this file when it changes. Normal builds reuse matching locks; a missing selection resolves the latest stable release once. Updating is explicit and excludes prereleases and development branches.

## Commands

```bash
python tools/ci/update_dependencies.py --help
python tools/ci/update_dependencies.py
python tools/ci/update_dependencies.py --only mira
python tools/ci/update_dependencies.py --only json --only mira
python tools/ci/update_dependencies.py --version json=3.12.0 --version openssl=4.0.3
python tools/ci/update_dependencies.py --only json --only mira --version json=3.12.0
```

The available, case-sensitive names are: `aria`, `doctest`, `json`, `mira`, `openssl`. Repeat `--only` for multiple selections and `--version` for different overrides. When using both, select every overridden name with `--only`. Unknown names, duplicate overrides and overrides outside the selection are errors.

To keep two dependencies fixed while updating the third, add `"version": "3.12.0"` to the existing JSON entry and `"version": "4.0.3"` to the existing OpenSSL entry; preserve all their source fields. Leave `mira` without a `version` field. A plain updater run then respects the two fixed versions and selects the latest stable release for each unpinned dependency. `--only mira` instead changes only Mira, leaving every other record untouched. No script edits are needed.

Precedence: this invocation's `--version` overrides the manifest; explicit manifest versions override defaults. Without an explicit request, ordinary resolution reuses the lock and deliberate updating discovers the latest stable version. A command-line selection remains locked when the manifest does not explicitly request a different version. If it does, the next resolution without that override restores the manifest request. Command-line overrides do not change persistent manifest requirements; record them there if they should constrain later updates. To unpin a library, remove its manifest `version` and deliberately update it.

## Fetch, build, verify and commit

The updater atomically saves selection metadata; it does **not** compile the entire application or establish API compatibility. After it succeeds:

```bash
python tools/ci/fetch_aria.py
cmake -S Workbench -B build/flavors/dependency-check -DCMAKE_BUILD_TYPE=Release
cmake --build build/flavors/dependency-check --config Release --parallel 3
bash Workbench/scripts/gen-web.sh probe  # Web shell integration; see README for each platform probe
```

Follow the [README](../README.en.md) for platform SDK selection and additional probes. Use the same configuration for build and CTest. Review `git diff -- dependencies.json` and commit the updated dependency file only after the build/tests succeed. Do not commit ignored downloads, source caches or build-directory effective locks. CI and releases consume checked-in selections.

## Existing locks, offline operation and overrides

```bash
python tools/ci/dependencies.py resolve --file dependencies.json
python tools/ci/dependencies.py resolve --file dependencies.json --offline
```

`resolve` fills missing/mismatched entries and preserves valid selections. Its offline mode requires a matching lock; successful offline resolution does not ensure source archives are cached. `update --offline` cannot discover new releases. A missing `resolved` entry causes fresh resolution for that dependency. Prefer the updater for controlled upgrades; deleting the whole `dependencies.json` also removes the source declarations needed for resolution.

`fetch_aria.py` accepts `--version`, `--update`, `--offline`, `--file`, and `--source`. `--version` overrides `ARIA_DEP_ARIA_VERSION`; `--source` overrides `ARIA_SOURCE`. A local Git source must still contain the selected full commit. Dirty dependencies are never overwritten; successful checkout replacements preserve the old tree under `build/deps/aria-backup-*`.

Aria source builds support CMake library overrides such as `-DARIA_DEP_JSON_VERSION=3.12.0`; these use a temporary build-directory result and preserve the source `dependencies.json`. Clear a cached override to return to the project selection. Explicit source directories or preexisting dependency targets take precedence and remain the caller's responsibility. Qt is an installed SDK: `-DARIA_DEP_QT_VERSION=6.10.0` selects an exact installed version; otherwise CMake searches its visible locations while respecting `Qt6_DIR`, `CMAKE_PREFIX_PATH`, and toolchains. This updater does not install Qt.

## Errors and rollback

Resolution failures leave the existing lock unchanged. Correct invalid names/versions, network failures or API limits and retry; do not disable integrity checks. A successful resolution followed by a failed build means compatibility still needs work. Restore `dependencies.json` from a known-good Git revision after preserving local changes, then repeat fetch/build/tests. Restoring metadata alone does not restore binaries. Never edit checksums to accept changed bytes.

Additional arguments: `--file PATH`, `--cache-dir PATH`, and `--offline`. Use the same file with fetch/update tools and `-DARIA_DEPENDENCIES_FILE=...` in Aria CMake source integrations. CMake keeps temporary overrides in the build directory and preserves the source dependency file. Run `--help` for the complete interface.

## Android compatibility profiles

The C++ updater above does not update Maven dependencies or require an Android
SDK. Android has a separate, validated update entry point:

```bash
python tools/ci/android_dependencies.py resolve
python tools/ci/android_dependencies.py update
python tools/ci/android_dependencies.py update --version activity=1.13.0
python tools/ci/android_dependencies.py check
```

`resolve` reuses the validated lock; `update` selects stable Compose BOM,
Activity, and Lifecycle releases and validates the whole toolchain combination.
The single `Workbench/platform/android/android-dependencies.json` contains
profile definitions, explicit requests, and the `resolved` validation record.
Successful explicit requests persist in that file. `check` needs no
network or SDK. AGP, Gradle, Kotlin, SDK, and NDK come from a reviewed profile,
not independent floating versions. The current profile uses AGP 9.4, Gradle 9.6,
Kotlin 2.4.20, JDK 17, compile SDK 37, and NDK 29.

Install the required SDK packages separately and first build matching native
archives with `bash Workbench/scripts/gen-android.sh`. Set `ANDROID_SDK_ROOT`
and `JAVA_HOME` to select installations. Candidate updates build Debug and
Release APKs in a temporary project and replay them offline with strict
verification before replacing profile locks, native Gradle locks, Maven checksum
metadata, and the Gradle distribution checksum. Concurrent updates are rejected;
a failed or interrupted transaction preserves the previous locks. Review and
commit that dependency document and Gradle's native lock/checksum files together.
The native files describe the full transitive graph in Gradle's required formats,
not a second custom copy of version requests. See
[Android dependencies](android-dependencies.md) for exact paths, override
precedence, and official compatibility references.
