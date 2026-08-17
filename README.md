<div align="center">

# ✦ AriaTools

**Aria 的跨平台 MVVM 最佳实践** · 插件化 · 模块化 · View 零逻辑

一份 C++20 核心，驱动 Qt / iOS / Android / Web 四个 View 壳

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Framework](https://img.shields.io/badge/Aria-v1.1.0-blueviolet.svg)](https://github.com/dqsjqian/Aria)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Qt6%20%7C%20iOS%20%7C%20Android%20%7C%20Web-lightgrey.svg)](#)

[English](README.en.md) | [简体中文](README.md)

</div>

---

## 🎯 这是什么？

**AriaTools**（原 AiTools）是 [Aria](https://github.com/dqsjqian/Aria)（C++20 响应式 MVVM 框架）的**旗舰级跨平台示例工程**，也是 Aria 跨平台架构的**最佳实践样板**：

- **一份纯 C++ 核心（Model + ViewModel + Service），四个平台 View 壳**（Qt6 桌面 / iOS UIKit / Android Compose / Web HTTP）
- **插件化模块化**：业务模块各自独立成库（一个模块一个 `wb_module_<name>` 静态库），**热插拔**——新增模块只需加目录，删除模块只需删目录，**不改任何核心文件**，CMake 自动发现
- **模块间零耦合**：模块间通过 EventBus 通信（publish/subscribe），无直接依赖；跨模块事件演示：cart 加商品 → dashboard/chat/notes 三模块同时收到通知
- **逻辑全部下沉**：所有业务逻辑在跨平台层（VM/Model/Service），View 层只做绑定与展示——View 里不允许出现业务计算、状态判断、硬编码文案
- **View 零逻辑由架构保证**：各平台 View 通过统一的注册表机制（QtViewFactory / UIViewFactory / ComposeViewFactory）按模块 id 取页面，模块自注册

> 一句话：**AriaTools 演示「一个 ViewModel 跑四个平台」——ViewModel 一份 C++ 代码，Qt / iOS / Android / Web 各自只写自己的 View 壳。**

## ✨ 核心特性

- 🧩 **热插拔模块架构** —— `IModule` 契约 + `make_<mod>_module()` 工厂；CMake **自动扫描** `modules/` 目录生成模块列表（`GeneratedModuleList.h`），新增模块只需加目录、删除模块只需删目录，**不改任何核心文件**
- 📦 **一个模块一个库** —— `wb_add_module()` 单行声明；SOURCES（跨平台逻辑）+ QT_SOURCES / IOS_SOURCES / Android 页面（平台 View）按平台条件编译；每个模块可独立编译为静态库
- 🔓 **模块间零耦合** —— 模块间通过 `EventBus` 通信（publish/subscribe），无直接依赖；演示：cart 加商品 → dashboard/chat/notes 三模块同时收到通知
- 🎛 **强类型 MVVM** —— View → ViewModel → Model → Service → 基础设施，依赖注入走 `ServiceHub`/DI Container，无 Service Locator、无全局单例
- 🌍 **国际化** —— XML i18n，运行时切语言，VM 文案属性自动刷新（`BaseVm::text()`）
- 🔌 **平台服务注入** —— UI 线程 executor / 工作线程池 / 延时调度器由各平台壳注入 `ServiceHub`，模块经 `ModuleContext` 获取，业务代码零平台依赖
- 🖥 **四平台 View 壳** —— Qt6（桌面）、iOS（UIKit）、Android（Compose + JNI side-channel）、Web（HTTP adapter，规划中）

## 🏗 架构分层

```
AriaTools/
├── Workbench/
│   ├── core/                     # ★ 纯 C++，零平台 UI 依赖
│   │   ├── utils/                #   wb_utils    工具函数
│   │   ├── infra/                #   wb_infra    稳定服务（i18n/storage/settings/…）+ DI + EventBus
│   │   ├── module_api/           #   wb_module_api 模块契约（IModule/ModuleContext/BaseVm）
│   │   └── app/                  #   wb_core_app 组装层（AppCore + ModulesManifest）
│   ├── modules/<mod>/            # ★ 业务模块，每个一个静态库
│   │   ├── viewmodels/           #   VM：全部业务逻辑（Property/Computed/Command）
│   │   ├── models/ services/     #   Model / Service
│   │   ├── module/               #   IModule 实现 + 工厂
│   │   ├── platforms/qt/         #   Qt View（QT_SOURCES）
│   │   ├── platforms/ios/        #   UIKit View（IOS_SOURCES）
│   │   ├── platforms/android/    #   Compose 页面（Gradle sourceSets 引入）
│   │   └── assets/i18n/          #   模块文案
│   └── platform/
│       ├── qt/                   #   Qt 壳（shell + QtViewFactory + UiHelpers）
│       ├── ios/                  #   iOS 壳（shell + UIViewFactory + IosUi）
│       └── android/              #   Android 工程（Gradle + JNI 桥 + ComposeViewFactory）
└── third_party/aria              # Aria 框架（submodule）
```

**数据流（JNI side-channel 示意，各平台同构）**：

```
C++ VM（aria::Property）→ on_changed → JNI 回调 → Kotlin StateFlow → Compose 重组
```

## 📦 模块清单（15）

| 模块 | 说明 | 演示的 Aria 能力 |
|---|---|---|
| dashboard | 首页概览 | Property / i18n |
| notes / calendar / tools | 记事 / 日历 / 小工具 | ObservableList / 表单 |
| settings / sync | 设置 / 同步 | 服务注入 / EventBus |
| tipcalc | 小费计算器 | Computed / Command / reactive::batch |
| unitconvert | 单位换算 | Computed 自动追踪 |
| cart | 购物车 | ObservableList 派生集合 |
| signup | 注册表单 | FormField / FormValidator |
| search | 搜索框 | debounce / 延时调度 |
| login | 模拟登录 | AsyncCommand / executor 注入 |
| chat | 聊天室 | EventBus 跨模块通信（多 VM：Publisher + Subscriber） |
| theme | 主题切换 | Container DI |
| wizard | 注册向导 | 多步表单状态机（3 子 VM + Navigator） |
| echo | 热插拔演示 | 最小化模块模板 |

## 🔌 模块热插拔

AriaTools 的模块系统支持**热插拔**——新增、删除模块**不需要修改任何核心文件**。

**CMake 自动发现**：`Workbench/CMakeLists.txt` 用 `file(GLOB)` 扫描 `modules/*/CMakeLists.txt`，自动生成 `GeneratedModuleList.h`，`ModulesManifest.cpp` 和各平台 `ViewManifest` 通过宏展开自动注册。

### 新增模块

```bash
# 1. 创建模块目录
mkdir -p Workbench/modules/mymod/{module,viewmodels,platforms/qt,assets/i18n}

# 2. 写 CMakeLists.txt（单行声明）
cat > Workbench/modules/mymod/CMakeLists.txt << 'EOF'
wb_add_module(NAME mymod
    SOURCES module/MyModModule.cpp
    QT_SOURCES platforms/qt/MyModView.cpp
)
EOF

# 3. 写 IModule + VM + View（参考 modules/echo/ 最小模板）
# 4. 重新构建——CMake 自动发现，无需改 ModulesManifest / ViewManifest / CMakeLists
```

### 删除模块

```bash
rm -rf Workbench/modules/mymod
# 重新构建——CMake 自动移除，编译不报错
```

### 模块间通信（零耦合）

模块间通过 `EventBus` 通信，无直接依赖：

```cpp
// cart 模块发布事件
bus.publish(wb::shared::events::ItemAddedToCart{"Apple", 3.5, 1});

// dashboard 模块订阅（不知道谁发的）
bus.subscribe<wb::shared::events::ItemAddedToCart>([](const auto& ev) {
    cartBadge.set("Cart: " + std::to_string(ev.qty));
});
```

演示链：cart 加商品 → dashboard 徽章刷新 + chat 系统消息 + notes 自动创建日志。

## 🚀 快速开始

```bash
# 克隆（含 Aria submodule）
git clone --recurse-submodules https://github.com/dqsjqian/AriaTools.git
cd AriaTools

# Qt 桌面（macOS / Linux）
bash Workbench/scripts/gen-mac.sh            # 或 gen-win.ps1 / build.ps1（Windows）

# iOS（需 Xcode）
bash Workbench/scripts/gen-ios.sh

# Android（需 NDK r26+ / SDK CMake 3.22.1）
bash Workbench/scripts/gen-android.sh        # 仅核心静态库
bash Workbench/scripts/gen-android.sh --apk  # 核心 + Gradle 打包 APK
```

## 🛠 技术栈

| 领域 | 技术 |
|---|---|
| 语言 | C++20 |
| 框架 | [Aria](https://github.com/dqsjqian/Aria)（vendored，C++20 MVVM） |
| 桌面 | Qt6（macOS / Windows / Linux） |
| iOS | UIKit（Xcode 工程） |
| Android | Kotlin + Jetpack Compose + NDK/JNI |
| Web | Aria HTTP adapter（规划中） |
| 构建 | CMake + Gradle + Xcode |

## 📜 License

MIT © dqsjqian
