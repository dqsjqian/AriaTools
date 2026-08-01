# AiTools / Workbench

> 跨平台工作台应用，基于 [Aria](https://github.com/dqsjqian/Aria) C++20 MVVM 框架构建。

[English](README.md)

---

## 项目简介

**Workbench** 是一个跨平台个人工作台，核心理念是 **一份 C++ 核心（Model + ViewModel + Service），多套平台 View 壳**。当前已实现 macOS（Qt6）和 iOS（UIKit）两端，Android（JNI）和 Windows（Qt）为后续阶段规划。

### 核心特性

- **C++20 核心，零平台 UI 依赖** — 纯 C++ 编译产物中无任何 Qt/UIKit 符号，多端复用的前提。
- **模块化架构** — 六大业务模块（Dashboard / Notes / Calendar / Tools / Settings / Sync），一模块一库，可独立构建。
- **强类型 MVVM** — View → ViewModel → Module Model → Module Service → Core Infrastructure，无 Service Locator、无全局 Singleton。
- **国际化** — 基于 XML 的 i18n，运行时切换语言。
- **Git 数据同步** — 所有数据在单一根目录，整体作为 git 仓库同步；用户自行配置私有仓库地址与 Token。
- **Apple 原生加密** — 使用 CommonCrypto + Security 框架。

---

## 技术栈

| 领域 | 技术 |
|---|---|
| 语言 | C++20 |
| 框架 | [Aria](https://github.com/dqsjqian/Aria)（vendored，C++20 MVVM） |
| 桌面端 | Qt6（macOS / Windows / Linux） |
| 移动端 | UIKit（iOS），JNI（Android，规划中） |
| 构建 | CMake ≥ 3.20 |
| 同步 | libgit2（接口已预留） |
| 加密 | CommonCrypto + Security（Apple），Stub（其他平台） |

---

## 项目结构

```
AiTools/
├── Workbench/                    主项目
│   ├── core/                     纯 C++20 核心（零平台 UI 依赖）
│   │   ├── app/                  应用装配层（AppCore + ModulesManifest）
│   │   ├── infra/                基础设施层（i18n/storage/secret/crypto/settings/sync/log）
│   │   ├── module_api/           模块契约（IModule / ModuleContext / ModuleRegistry）
│   │   └── utils/                工具函数（Base64 / 平台宏）
│   ├── modules/                  业务模块（一模块一库）
│   │   ├── _shared/              跨模块共享资源
│   │   ├── dashboard/            首页概览
│   │   ├── notes/                记事本（Markdown，一条笔记一个 .md 文件）
│   │   ├── calendar/             日历（.ics 第三方订阅）
│   │   ├── tools/                小工具（Base64 / 随机串 / JSON / 文件加解密）
│   │   ├── settings/             设置（含同步配置）
│   │   └── sync/                 数据同步
│   ├── platform/                 各端 View 壳
│   │   ├── qt/                   Qt6 桌面（Win/Mac/Linux）
│   │   ├── ios/                  iOS UIKit
│   │   └── android/              Android JNI（预留）
│   ├── scripts/                  一键生成/构建脚本
│   ├── docs/                     架构文档 & 开发状态
│   └── CMakeLists.txt
├── third_party/
│   └── aria/                     Aria 框架（git submodule）
├── .gitmodules
├── .gitignore
└── aicoding.config.md            AI 辅助编码配置
```

---

## 模块说明

| 模块 | 说明 | 状态 |
|---|---|---|
| Dashboard | 首页概览 | 占位 |
| Notes | 记事本，Markdown 图文增删改查 | ✅ 已实现 |
| Calendar | 日历，.ics 第三方订阅 | 占位 |
| Tools | Base64 / 随机串 / JSON / 文件加解密 | ✅ 已实现 |
| Settings | 设置，含同步配置（双向绑定） | ✅ 可编辑 |
| Sync | 数据同步，调用 ISyncService | 桩实现 |

---

## 架构设计

### 分层

```
Platform View（QWidget / UIView）
    ↓ Binding / Command
ViewModel
    ↓ 强类型调用与状态订阅
Module Model（模块唯一业务状态源）
    ↓
Module Service Interface
    ↓
Service Implementation
    ↓
Core Infrastructure（Storage / HTTP / Crypto / Git）
    ↓
文件系统 / 网络 / DB / 系统 API
```

### 关键设计原则

1. **Core 绝不依赖平台 UI** — `workbench_core` 只链 Aria binding/runtime/async/core，编译产物中无 Qt/UIKit 符号。
2. **每端外壳 = 适配器 + BindingEngine + AppCore** — 换平台只需换一套 View + 适配器，`AppCore` 与所有 VM 原样复用。
3. **模块内共享 Model** — 同模块多 VM 通过共享 Model 协作；跨模块使用 EventBus。
4. **构造函数注入** — Model/Service 由 Module 创建并注入，不使用 Service Locator 或全局 Singleton。
5. **一条笔记一个文件** — 记事本采用 `notes/<uuid>.md`，从根源规避多端 git 冲突。

---

## 快速开始

### 前置条件

- CMake ≥ 3.20
- C++20 兼容编译器（Clang 15+ / MSVC 2022+）
- Qt6（桌面端）/ Xcode（iOS 端）
- git（含 submodule 支持）

### 克隆

```bash
git clone --recurse-submodules <repo-url> AiTools
cd AiTools
```

### 构建 macOS 桌面应用

```bash
bash Workbench/scripts/gen-mac.sh run
```

### 构建 iOS 应用

```bash
# 生成并打开 Xcode 工程
bash Workbench/scripts/gen-ios.sh open

# 或直接构建
bash Workbench/scripts/gen-ios.sh build
```

### 运行模块测试

```bash
# Tools 模块测试
cmake -S Workbench/modules/tools/tests -B build/mac/modules/tools
cmake --build build/mac/modules/tools
ctest --test-dir build/mac/modules/tools --output-on-failure
```

### 后续平台（规划中）

```bash
# Windows (Qt6) — 预留
Workbench/scripts/gen-win.ps1

# Android (JNI) — 预留
bash Workbench/scripts/gen-android.sh

# Web (HTTP) — 预留
bash Workbench/scripts/gen-web.sh
```

---

## CMake 构建选项

| 选项 | 默认值 | 说明 |
|---|---|---|
| `WORKBENCH_TARGET_QT` | `ON` | 构建 Qt6 桌面应用 |
| `WORKBENCH_TARGET_IOS` | `OFF` | 构建 iOS UIKit 应用 |

```bash
# 示例：仅构建 iOS
cmake -S Workbench -B build/ios \
  -DWORKBENCH_TARGET_QT=OFF \
  -DWORKBENCH_TARGET_IOS=ON \
  -G Xcode -DCMAKE_SYSTEM_NAME=iOS
```

---

## 开发状态

### 已完成

- 六模块骨架及 Qt/macOS、UIKit/iOS 外壳
- 稳定基础设施接口：i18n / storage / settings / sync / secret / crypto
- Tools：Base64、随机字符串、JSON 格式化/压缩、Apple 原生文件加解密
- Notes：完整 CRUD（Markdown front matter + 附件管理 + EventBus 通知）
- Mac 完整 App 构建通过，iOS 构建通过
- Tools & Notes CTest 全部通过

### 进行中

- Calendar 真实业务（.ics 订阅）
- Settings/Sync 接入 libgit2 真实同步

### 后续规划

1. 补齐平台：Android (JNI) / Windows (Qt) / Web (HTTP)，均复用 `core/`
2. Git LFS 支持
3. 可选端到端加密

---

## 第三方依赖

| 依赖 | 说明 |
|---|---|
| [Aria](https://github.com/dqsjqian/Aria) | C++20 MVVM 框架，以 git submodule 方式引入 |

---

## License

个人学习项目，暂未选择开源协议。
