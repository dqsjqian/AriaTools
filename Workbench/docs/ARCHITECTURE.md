# Workbench 架构说明

跨平台工作台，基于 [Aria](https://github.com/dqsjqian/Aria) C++20 MVVM 框架。
核心理念：**一份 C++ 核心（Model + ViewModel + Service），多套平台 View 壳**。

## 分层

```
Workbench/
├── core/                      纯 C++20 核心（零平台 UI 依赖）——所有平台共享
│   ├── app/AppCore            装配根：拥有 Services + 6 个模块 VM
│   ├── modules/<mod>/         每个模块的 Model + ViewModel
│   └── services/              数据层抽象接口 + 桩实现
│       ├── I*.h               IStorage / ISettings / ISync / ISecretStore / ICrypto
│       └── stub/              非目标平台的最小回退；发布前替换为真实平台实现
├── platform/                  各端 View 壳，结构对称，各自持有平台适配器
│   ├── qt/                    Qt6 桌面（Win/Mac/Linux 共用）：入口 + AppShell + Views
│   ├── ios/                   iOS UIKit：入口 + IosShell + Views（.mm）
│   ├── android/               预留（JNI，见 README）
│   └── win/                   预留（复用 qt/，仅打包脚本）
├── scripts/                   各平台一键生成/构建脚本
│   ├── gen-mac.sh             Mac (Qt6)      ✅ 可用
│   ├── gen-ios.sh             iOS (UIKit)    ✅ 可用（Xcode 工程）
│   ├── gen-win.ps1            Windows        预留
│   ├── gen-android.sh         Android        预留
│   └── gen-web.sh             Web (HTTP)     预留
└── third_party/aria           vendored 框架（add_subdirectory）
```

## 关键设计

### 1. core 绝不依赖平台 UI
`workbench_core` 只链 `aria::binding/runtime/async/core`，编译产物中无任何
Qt/UIKit 符号。这是「多端复用」成立的前提，已用 `nm` 验证。

### 2. 每端外壳 = 适配器 + BindingEngine + AppCore
| 端 | 适配器 | Dispatch 策略 | View |
|----|--------|--------------|------|
| Qt | `aria::qt6::QtAdapter` | SmartMarshal（有 QtDispatcher） | QWidget |
| iOS | `aria::adapters::uikit::UIKitAdapter` | Direct（UIKit 回调即主线程） | UIView |

外壳把 `core_.<module>()` 的 VM 通过 `be.bind_text/bind_command/...` 绑到原生控件。
换平台 = 换一套 View + 适配器，`AppCore` 与所有 VM 原样复用。

### 3. 数据统一目录 + git 同步（面向多用户分发）
- 所有数据在单一根目录（默认 `~/WorkbenchData`，可配），整体作为一个 git 仓库。
- git 仓库地址/分支/用户名/Token 全部是**运行时用户设置**（`SyncSettings`），
  绝不写死——每个用户填自己的私有 Gitee 仓库。Token 走 `ISecretStore` 安全存储。
- 全自动同步（定时 + 条件触发）、opt-in 加密（可选范围）、Git LFS：接口已预留，
  同步使用 libgit2；Apple 端加密使用 CommonCrypto + Security 原生框架。
- 记事本采用「一条笔记一个 .md 文件（UUID 命名）」，从根上规避多端 git 冲突。

## 模块内 MVVM 分层

Workbench 采用以**模块级共享 Model**为状态中心的强类型 MVVM：

```text
Platform View
    ↓ Binding / Command
ViewModel
    ↓ 强类型调用与状态订阅
Module Model
    ↓
Module Service Interface
    ↓
Service Implementation
    ↓
Core Infrastructure
    ↓
文件系统 / 网络 / DB / Git / 系统 API
```

职责和依赖规则：

- **View** 只负责平台控件与绑定，不调用 Model、Service 或基础设施。
- **ViewModel** 只维护页面展示/交互状态，将用户操作转给 Module Model；VM 之间不直接互调。
- **Module Model** 是模块唯一业务状态源，封装业务操作、协调 Service，并供模块内多个 VM 共享。
- **Module Service** 提供数据访问边界，隐藏文件格式、网络协议、缓存和数据库细节。
- **Core Infrastructure** 提供不含业务语义的稳定能力，如存储、网络、加密、密钥和 Git。
- 纯数据类型使用 `Note`、`NoteId` 等命名，不使用 `NoteModel`，避免与 MVVM Model 混淆。
- Model/Service 由 Module 创建并通过构造函数注入；它们可以在模块作用域内唯一，但不得实现为静态全局 Singleton。
- 每个模块的静态资源归入 `assets/`：文案位于 `assets/i18n/`，模块图标位于 `assets/icons/`；只创建实际需要的资源目录。
- 全模块共享资源位于 `modules/_shared/assets/`，源码目录不再重复增加 `common/`；构建时映射为运行时 `common` 命名空间。
- 同模块多 VM 通过共享 Model 协作；跨模块事实通知使用 EventBus，跨模块能力使用显式接口。
- 默认不增加 Domain、ApplicationService、Repository 或 BizModel 空转层；复杂编排真实出现后再从 Model 抽取 UseCase/Policy。
- 使用强类型方法、结果和事件，不采用 Service Locator 或 `Variant + 数字 ID` 作为常规业务接口。

以 Notes 为例：

```text
NotesView → NotesVm → NotesModel → INotesService
                                   ↓
                         MarkdownNotesService
                                   ↓
                            IStorageService
```

生命周期由应用装配树明确持有：

```text
AppCore
  └── NotesModule
        ├── MarkdownNotesService（模块作用域实例）
        ├── NotesModel（模块作用域共享实例）
        └── Notes VM（共享 NotesModel）
```

## 6 个模块
| 模块 | 说明 | 骨架状态 |
|------|------|---------|
| Dashboard | 首页概览 | 占位 |
| Notes | 记事本（图文增删改查） | 空列表 + 增删占位 |
| Calendar | 日历（.ics 第三方订阅） | 订阅入口占位 |
| Tools | 小工具 | base64/随机串**已可用**；json/文件加解密占位 |
| Settings | 设置（含同步配置） | 可编辑，双向绑定 |
| Sync | 数据同步 | 调 ISyncService 桩（模拟） |

## 构建

```bash
# Mac 桌面（Qt6）
bash Workbench/scripts/gen-mac.sh run

# iOS 模拟器（需完整 Xcode；脚本会自动用 DEVELOPER_DIR 指向 Xcode）
bash Workbench/scripts/gen-ios.sh open   # 生成并打开 Xcode
```

## 后续阶段
1. 业务实现：Tools → Notes → Calendar → Settings/Sync（Apple 加密使用 CommonCrypto，同步接 libgit2）。
2. 补齐平台：Android(JNI) / Windows(Qt) / Web(HTTP)，均复用 `core/`。
