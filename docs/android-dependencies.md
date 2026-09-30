# Android dependency profiles

Android follows the same policy as the C++ dependencies: explicit versions take
priority, the first resolution chooses stable library releases, ordinary builds
reuse the lock, and upgrades are deliberate. SDK installation remains the user's
or CI runner's responsibility.

The Android graph has an additional compatibility constraint. Compose libraries,
Activity, Lifecycle, Kotlin, AGP, Gradle, and the compile SDK must work together.
The single `Workbench/platform/android/android-dependencies.json` contains the
reviewed toolchain families, the selected family, optional exact version requests,
and a `resolved` record with actual versions and validation hashes. Request and
profile fingerprints invalidate stale results without copying their full inputs
into a second custom lock file. The default family uses AGP 9.4, its required Gradle 9.6, Kotlin 2.4.20,
JDK 17, compile SDK 37, and NDK 29. It selects the latest stable patch in that AGP
family and the latest stable Compose BOM, Activity, and Lifecycle releases.
Adding a new toolchain family requires reviewing its compatibility and validating
an APK build. A newer major toolchain is never silently substituted.

The compatibility references are the official [AGP table](https://developer.android.com/build/releases/agp-9-4-0-release-notes),
[Kotlin table](https://kotlinlang.org/docs/gradle-configure-project.html),
[AGP built-in Kotlin migration](https://developer.android.com/build/migrate-to-built-in-kotlin),
and [Compose BOM guidance](https://developer.android.com/develop/ui/compose/bom).
Kotlin's fully supported AGP range currently ends at 9.3.1; the 9.4 profile is
accepted only after this project's complete build and strict locked replay pass.

From the repository root:

```sh
# Reuse the validated lock; resolve and validate if this is the first run.
python tools/ci/android_dependencies.py resolve

# Re-resolve stable library releases and validate the complete candidate graph.
python tools/ci/android_dependencies.py update

# Explicit request overrides the manifest and environment.
python tools/ci/android_dependencies.py update --version activity=1.13.0

# Verify lock integrity without accessing the network or requiring an SDK.
python tools/ci/android_dependencies.py check

# Print the SDK package names CI must install for the selected lock.
python tools/ci/android_dependencies.py sdk-packages
```

`--profile` overrides `ARIA_ANDROID_PROFILE`, which overrides the manifest.
`--version NAME=VERSION` overrides `ARIA_DEP_ANDROID_<NAME>_VERSION`, which
overrides the manifest. Supported library names are `compose_bom`, `activity`,
and `lifecycle`; toolchain fields are `agp`, `kotlin`, `gradle`, `compile_sdk`,
`build_tools`, `ndk`, `cmake`, and `java`. Successful resolution saves explicit
requests in the manifest so that the next ordinary build retains them. Overrides
must still pass the selected profile's compatibility checks and APK build.

Updates require installed SDK packages, a suitable JDK, and the native archives
produced by `bash Workbench/scripts/gen-android.sh`. Set `ANDROID_SDK_ROOT` and
`JAVA_HOME` to select installations. No global SDK, JDK, or user configuration is
installed or changed by the resolver. An update builds Debug and Release APKs in
a temporary Gradle project, writes native Gradle dependency locks and SHA256
verification metadata, then repeats the builds offline with strict verification.
Only after both passes succeed are `resolved` and the native Gradle files replaced.
A failed candidate leaves the existing dependency document and Gradle files intact.

Commit the single Android dependency document and Gradle's native consumer files together:

- `android-dependencies.json`
- `gradle/wrapper/gradle-wrapper.properties` (including the official distribution checksum)
- `gradle/verification-metadata.xml`
- `buildscript-gradle.lockfile`
- `app/gradle.lockfile`

All paths above are relative to `Workbench/platform/android`. The `resolved`
record hashes the validated Gradle files. Native Gradle locks and checksum
metadata describe the complete transitive graph in the formats Gradle consumes;
they are not additional custom copies of version requests. Ordinary resolution rejects a
modified or incomplete validation record; it does not silently refresh it.
Checksums for newly selected Maven artifacts are recorded from the configured
HTTPS repositories during an explicit update and should be reviewed as part of
that update. Subsequent builds enforce those recorded checksums. CI also checks
the packaged native libraries' 16 KB alignment.

The app's target SDK is a separate application behavior choice. Updating the
compile SDK to satisfy a dependency does not change the target SDK automatically.

When changing NDK/CMake through explicit version requests, select those same
requests while building the native stage (for example,
`ARIA_DEP_ANDROID_NDK_VERSION=... bash Workbench/scripts/gen-android.sh`). The
resolver verifies the NDK used by the native cache before compiling the JNI
bridge. Stage one also exports the selected Aria source path and JSON include
root, so a local CMake source override cannot mix one version's headers with
another version's archives. These machine-specific manifests stay in ignored
build output and are never committed.
