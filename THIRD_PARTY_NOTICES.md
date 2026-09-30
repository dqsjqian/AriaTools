# Third-party notices

AriaTools' original code is MIT-licensed; see [LICENSE](LICENSE). Dependencies retain their own licenses. This file identifies the reviewed scope and distribution materials; it does not relicense third-party code or provide legal clearance for every distribution.

## Native and desktop dependencies

| Component | Reviewed selection | Use | License and source |
|---|---|---|---|
| Aria | 3.1.0 | Application runtime and adapters | MIT; selected checkout's `LICENSE`, [upstream](https://github.com/dqsjqian/Aria) |
| nlohmann/json | 3.12.0 | Native application data and HTTP serialization | MIT; `LICENSE.MIT` plus all header SPDX copyright notices, [upstream](https://github.com/nlohmann/json/tree/v3.12.0) |
| Mira | 1.0.0 | Web HTTP/1 transport through Aria | MIT; [upstream license](https://github.com/dqsjqian/Mira/blob/v1.0.0/LICENSE) and [third-party notices](https://github.com/dqsjqian/Mira/blob/v1.0.0/THIRD_PARTY_NOTICES.md) |
| Qt 6 Core/Gui/Widgets | Installed SDK selected by CMake | Qt desktop shell | The selected Qt distribution's LGPL/GPL/commercial terms and bundled notices apply; [Qt licensing](https://doc.qt.io/qt-6/licensing.html) |
| OpenSSL | 4.0.3 in dependency selection | Optional TLS dependency; the current Workbench Web target disables TLS | Apache-2.0; [upstream](https://github.com/openssl/openssl/tree/openssl-4.0.3) |
| doctest | 2.5.3 | Framework tests, not the application runtime | MIT, with embedded Boost-1.0 portions; [upstream](https://github.com/doctest/doctest/tree/v2.5.3) |

The JSON headers include notices for Niels Lohmann, Evan Nemerson, Florian Loitsch, Björn Hoehrmann, and The Abseil Authors. Android packaging extracts every SPDX copyright and license line from the actual selected headers rather than retaining only the top-level author.

The Workbench Web build uses Mira's base core, TCP transport, and HTTP/1 modules with TLS, WebSocket, HTTP/2, and HTTP/3 disabled. This selection does not link Mira's optional OpenSSL, zlib, nghttp2, ngtcp2, or nghttp3 dependencies. Enabling those features requires the corresponding upstream notices, including embedded components listed by Mira; Mira's own MIT text alone is insufficient for those expanded distributions.

Desktop distributors must preserve applicable notices and meet the terms of the Qt kit they distribute. Dynamic linking alone does not establish complete LGPL compliance; review replacement/relinking rights, license copies, and the corresponding library source requirements for the actual package. This project does not apply its MIT license to Qt or system SDKs.

## Android runtime

The reviewed Gradle graph uses Compose BOM 2026.09.00, Activity 1.13.0, Lifecycle 2.11.0, and Kotlin 2.4.20. Its 64 Release binary runtime components and two additional Debug-only Compose tooling components declare Apache-2.0 in publisher POMs. Platform/variant metadata is counted separately from binary libraries. A POM declaration is not proof that every embedded source file has that license.

`androidx.graphics:graphics-path:1.0.1` includes a native library and supplies no LICENSE/NOTICE text in its AAR or nested `classes.jar`. The [official 1.0.1 release history](https://developer.android.com/jetpack/androidx/releases/graphics#graphics-path-1.0.1) identifies source revision [8a05a22af450d589ef911d772a001a49dcb05b71](https://android.googlesource.com/platform/frameworks/support/+/8a05a22af450d589ef911d772a001a49dcb05b71/graphics/graphics-path/). Its three compiled C++ sources and included project headers carry Apache-2.0 notices (Android Open Source Project, including 2006, 2013, 2017, and 2022 notices). Its source accesses system Skia path layouts; the inspected build does not compile a bundled Skia source tree and explicitly uses `-nostdlib++`. This source review is specific to 1.0.1, not future versions or other AndroidX libraries.

Android builds generate `assets/licenses/` containing:

- AriaTools' own MIT license and the actual selected Aria MIT license.
- The actual selected JSON `LICENSE.MIT` and all exported header SPDX attributions.
- The complete [Apache-2.0 license](licenses/Apache-2.0.txt), covering the reviewed Apache-licensed AndroidX/Kotlin components, and this notice file.
- Original LICENSE/LICENCE/NOTICE/COPYING files found in the selected runtime JAR/AAR archives, including nested `classes.jar` entries.
- The selected NDK's original LLVM distribution and sysroot NOTICE files. The JNI library statically links the C++ runtime; the combined upstream notices also cover components that may not occur in this APK and do not imply that the APK includes all NDK tools. LLVM's [license and exceptions](https://llvm.org/docs/DeveloperPolicy.html#new-llvm-project-license-framework) remain applicable.
- `BUILD-LICENSES.json`, listing actual runtime coordinates, artifact hashes, copied notice hashes, JSON version, and NDK version, without local machine paths.

The generator does not choose dependency versions or automatically approve their licenses. It records actual build inputs and preserves available text. After dependency updates, review the new upstream terms and embedded notices; do not assume the reviewed Apache-2.0 scope above applies to newly selected components. A system-provided JSON installation without its source license can set `ARIA_JSON_LICENSE_FILE` to that installation's original license text.

## Build tools and publication scope

AGP, Gradle, Kotlin compiler plugins, JUnit, and their transitive build/test dependencies are separate from the APK runtime. Their graph includes permissive licenses and other terms: for example JNA 5.6.0 explicitly offers [LGPL-2.1-or-later OR Apache-2.0](https://github.com/java-native-access/jna/blob/5.6.0/LICENSE), juniversalchardet uses MPL-1.1, and JUnit tooling uses EPL-2.0. Some JAXB parent metadata lists EPL/GPL-with-Classpath-Exception terms. These are not automatically APK payload. Redistribution of a build-tool distribution needs its own notice review. The Android SDK also has separate [SDK terms](https://developer.android.com/studio/terms).

The current CI builds and tests desktop/mobile binaries but does not upload APKs or other application binaries as workflow artifacts or Release assets. GitHub's automatic source archives do not contain ignored build dependencies or generated APKs. Binary distributors should keep the generated license assets and the applicable desktop notices with their package; a source-only release is not evidence that a later binary package satisfies every dependency's obligations.
