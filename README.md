# AiTools / Workbench

> A cross-platform workbench application built on the [Aria](https://github.com/dqsjqian/Aria) C++20 MVVM framework.

[中文文档](README.zh_CN.md)

---

## Overview

**Workbench** is a cross-platform personal workbench. The core principle is **one C++ core (Model + ViewModel + Service), multiple platform View shells**. macOS (Qt6) and iOS (UIKit) are currently implemented; Android (JNI) and Windows (Qt) are planned for later phases.

### Key Features

- **C++20 core with zero platform UI dependency** — No Qt/UIKit symbols in pure C++ build artifacts, enabling true multi-platform reuse.
- **Modular architecture** — Six business modules (Dashboard / Notes / Calendar / Tools / Settings / Sync), one library per module, independently buildable.
- **Strongly-typed MVVM** — View → ViewModel → Module Model → Module Service → Core Infrastructure. No Service Locator, no global Singletons.
- **Internationalization** — XML-based i18n with runtime language switching.
- **Git-based data sync** — All data in a single root directory synced as a git repo; users configure their own private repo.
- **Apple native crypto** — Uses CommonCrypto + Security framework.

---

## Tech Stack

| Area | Technology |
|---|---|
| Language | C++20 |
| Framework | [Aria](https://github.com/dqsjqian/Aria) (vendored, C++20 MVVM) |
| Desktop | Qt6 (macOS / Windows / Linux) |
| Mobile | UIKit (iOS), JNI (Android, planned) |
| Build | CMake ≥ 3.20 |
| Sync | libgit2 (interface reserved) |
| Crypto | CommonCrypto + Security (Apple), Stub (other platforms) |

---

## Project Structure

```
AiTools/
├── Workbench/                    Main project
│   ├── core/                     Pure C++20 core (zero platform UI dependency)
│   │   ├── app/                  Application assembly (AppCore + ModulesManifest)
│   │   ├── infra/                Infrastructure (i18n/storage/secret/crypto/settings/sync/log)
│   │   ├── module_api/           Module contracts (IModule / ModuleContext / ModuleRegistry)
│   │   └── utils/                Utilities (Base64 / Platform macros)
│   ├── modules/                  Business modules (one library per module)
│   │   ├── _shared/              Cross-module shared assets
│   │   ├── dashboard/            Dashboard overview
│   │   ├── notes/                Notes (Markdown, one .md file per note)
│   │   ├── calendar/             Calendar (.ics third-party subscription)
│   │   ├── tools/                Tools (Base64 / random string / JSON / file encryption)
│   │   ├── settings/             Settings (including sync configuration)
│   │   └── sync/                 Data sync
│   ├── platform/                 Platform View shells
│   │   ├── qt/                   Qt6 desktop (Win/Mac/Linux)
│   │   ├── ios/                  iOS UIKit
│   │   └── android/              Android JNI (placeholder)
│   ├── scripts/                  One-click generate/build scripts
│   ├── docs/                     Architecture docs & development status
│   └── CMakeLists.txt
├── third_party/
│   └── aria/                     Aria framework (git submodule)
├── .gitmodules
├── .gitignore
└── aicoding.config.md            AI-assisted coding config
```

---

## Modules

| Module | Description | Status |
|---|---|---|
| Dashboard | Home overview | Placeholder |
| Notes | Markdown notes with full CRUD | ✅ Implemented |
| Calendar | Calendar with .ics subscription | Placeholder |
| Tools | Base64 / random string / JSON / file crypto | ✅ Implemented |
| Settings | Settings with sync config (two-way binding) | ✅ Editable |
| Sync | Data sync via ISyncService | Stub |

---

## Architecture

### Layering

```
Platform View (QWidget / UIView)
    ↓ Binding / Command
ViewModel
    ↓ Strongly-typed calls & state subscription
Module Model (sole business state source)
    ↓
Module Service Interface
    ↓
Service Implementation
    ↓
Core Infrastructure (Storage / HTTP / Crypto / Git)
    ↓
File System / Network / DB / System API
```

### Key Design Principles

1. **Core never depends on platform UI** — `workbench_core` only links Aria binding/runtime/async/core; no Qt/UIKit symbols in build artifacts.
2. **Each platform shell = Adapter + BindingEngine + AppCore** — Switching platforms means swapping View + Adapter; `AppCore` and all VMs are reused as-is.
3. **Shared Model within module** — Multiple VMs in the same module collaborate via a shared Model; cross-module communication uses EventBus.
4. **Constructor injection** — Model/Service created and injected by Module; no Service Locator or global Singleton.
5. **One note per file** — Notes use `notes/<uuid>.md` to fundamentally avoid multi-platform git conflicts.

---

## Getting Started

### Prerequisites

- CMake ≥ 3.20
- C++20 compatible compiler (Clang 15+ / MSVC 2022+)
- Qt6 (desktop) / Xcode (iOS)
- git (with submodule support)

### Clone

```bash
git clone --recurse-submodules <repo-url> AiTools
cd AiTools
```

### Build macOS Desktop App

```bash
bash Workbench/scripts/gen-mac.sh run
```

### Build iOS App

```bash
# Generate and open Xcode project
bash Workbench/scripts/gen-ios.sh open

# Or build directly
bash Workbench/scripts/gen-ios.sh build
```

### Run Module Tests

```bash
# Tools module tests
cmake -S Workbench/modules/tools/tests -B build/mac/modules/tools
cmake --build build/mac/modules/tools
ctest --test-dir build/mac/modules/tools --output-on-failure
```

### Future Platforms (Planned)

```bash
# Windows (Qt6) — placeholder
Workbench/scripts/gen-win.ps1

# Android (JNI) — placeholder
bash Workbench/scripts/gen-android.sh

# Web (HTTP) — placeholder
bash Workbench/scripts/gen-web.sh
```

---

## CMake Build Options

| Option | Default | Description |
|---|---|---|
| `WORKBENCH_TARGET_QT` | `ON` | Build Qt6 desktop app |
| `WORKBENCH_TARGET_IOS` | `OFF` | Build iOS UIKit app |

```bash
# Example: iOS only
cmake -S Workbench -B build/ios \
  -DWORKBENCH_TARGET_QT=OFF \
  -DWORKBENCH_TARGET_IOS=ON \
  -G Xcode -DCMAKE_SYSTEM_NAME=iOS
```

---

## Development Status

### Completed

- Six-module skeleton with Qt/macOS and UIKit/iOS shells
- Stable infrastructure interfaces: i18n / storage / settings / sync / secret / crypto
- Tools: Base64, random string, JSON format/compress, Apple native file encryption
- Notes: Full CRUD (Markdown front matter + attachment management + EventBus)
- macOS full app build passing, iOS build passing
- Tools & Notes CTest all passing

### In Progress

- Calendar real business (.ics subscription)
- Settings/Sync integration with libgit2

### Roadmap

1. Complete platforms: Android (JNI) / Windows (Qt) / Web (HTTP), all reusing `core/`
2. Git LFS support
3. Optional end-to-end encryption

---

## Third-Party Dependencies

| Dependency | Description |
|---|---|
| [Aria](https://github.com/dqsjqian/Aria) | C++20 MVVM framework, included as git submodule |

---

## License

Personal learning project, no open-source license selected yet.
