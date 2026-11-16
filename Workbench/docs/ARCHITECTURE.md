# Workbench 架构说明

跨平台工作台，基于 [Aria](https://github.com/dqsjqian/Aria) C++23 MVVM 框架。
核心理念：**一份 C++ 核心（Model + ViewModel + Service），多套平台 View 壳**。

## 目录与职责

```text
AriaTools/
├── Workbench/
│   ├── core/
│   │   ├── app/               AppCore、模块装配与导航
│   │   ├── infra/             i18n、存储、设置、同步等服务接口与实现
│   │   ├── module_api/        模块契约、上下文与跨模块能力
│   │   └── utils/             平台无关工具函数
│   ├── modules/<module>/      每模块的 Model、Service、VM、View 与测试
│   ├── platform/              Qt、UIKit、Android、Web 入口与平台壳
│   ├── cmake/                 模块注册与独立测试构建 helper
│   └── scripts/               各平台构建入口与共享 MSVC helper
├── scripts/ci/                  固定版本依赖获取与安全回归
├── docs/marketing/            有日期的发布介绍与配图
└── build/deps/aria/           Git 忽略的固定版本框架缓存
```

## 装配与平台边界

`core/` 的业务接口和 VM 不直接操作平台控件。`wb_core_app` 装配模块，
模块 CMake 根据目标平台选择 Qt / UIKit 等 View 源文件；平台壳负责
适配器、UI 调度和 BindingEngine。Android 的 Compose 入口由
`ModulePages.kt` 显式注册，Web 壳提供 HTTP / REST / SSE 访问。

Qt / UIKit 模块由 CMake 扫描 `modules/*/CMakeLists.txt` 生成的
`<build>/generated/app/GeneratedModuleList.h` 注册；增加模块时同时提供相应平台 View 入口。
每个构建目录持有自己的生成文件，不修改源码目录。

## 数据与服务实现

数据根目录默认是 `~/WorkbenchData`。Notes 使用 Markdown 文件持久化，
Apple 平台提供原生加解密；设置、同步、密钥和 HTTP 的默认实现中仍有
内存或 Stub 服务。远端仓库与凭据由用户在运行时配置，同步与安全存储
接口不等于已经接入真实 Git 同步或系统密钥链。
具体装配以 `core/infra/ServiceHub.cpp` 和 `ServiceFactories.h` 为准。

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

## 模块与构建

当前模块列表、平台依赖和完整构建命令统一维护在[仓库 README](../../README.md)。
平台脚本位于 `Workbench/scripts/`，所有构建产物进入根目录的 `build/`；
先运行 `python3 scripts/ci/fetch_aria.py` 获取固定版本的 Aria，再选择平台入口。

## Aria 提供的通用能力

本工程通过 ForgeRecipes 0.1.9 配置 Aria 3.3.0 的可选组件。正式版本与源码
revision 固定在依赖锁中；显式本地 Aria 源码覆盖仍可用于开发验证。

`aria::i18n` 拥有 XML 解析与缓存、语言与资源修订信号、fallback 查询、
`bind_text` / `bind_localized`。ServiceHub 显式持有 `LocaleContext`，通过
ModuleContext 注入每个 VM；不再有进程级 backend 指针。Workbench 只保留
`common` 域、`zh-CN` 默认语言、资源目录与 `source_location` 域推导策略。
查询顺序保持“模块当前语言 → 模块默认语言 → common 当前语言 → common 默认语言”；
后台操作先回到 UI executor，再翻译完成消息。`BaseVm` 是应用便利包装，
框架 text binding 本身不要求继承 VM。旧 `localize` 回调只随语言/资源变化重跑。

`aria::app` 拥有 `ModuleRegistry` 的标识与排序、`ViewModelProvider`
的主 VM 缓存与新实例工厂、单提供者 `MountRegistry`、`TypedRouteRegistry`、
可撤销 `ExtensionRegistry` 和 `ModuleLifecycle`。AppCore 用分阶段事务装配，
失败时撤销页面、路由、槽位与模块，可修复资源后重试；退出逆序释放注册。
Tools 的 `IModule` 是工作台页面贡献契约，框架模块不必有页面。JSON 能力接口、
`Push/Modal/Window` 元数据、导航标题与平台控件仍属于应用；导航栈直接复用 Aria Navigator。
平台 ViewFactory 复用通用注册表，但 QWidget / UIViewController builder 留在平台支持层。
根控件/控制器持有 View 包装，销毁时释放其订阅，避免槽位撤销通知访问已销毁页面。

注册句柄由装配层持有；丢弃句柄或显式撤销会移除后续解析能力。已经交付的 VM
按共享所有权继续存活，不表示二进制卸载。重复模块/路由注册被拒绝；替换路由需
先 `Unregister<I>()`。typed 路由或挂载返回的 VM 必须实现指定接口，否则解析失败。
槽位 enable/remove 变更会通知宿主，Dashboard 无需手工同步其他调用者的变更。

## 迁移回归

配置 `-DWORKBENCH_BUILD_TESTS=ON` 可在同一生产构建中运行六个业务模块测试，
以及 i18n 与 application 集成测试。集成测试遍历真实 XML，覆盖回退顺序、
独立 locale、订阅析构、资源重载、启动回滚与重试、主 VM 共享、槽位变更和异步文案。
`workbench --probe` 创建临时数据目录后实例化所有 Qt 页面；测试与 probe 不写个人
`~/WorkbenchData`。独立 i18n 工程位于 `Workbench/tests/i18n`，仅依赖安装后的 Aria SDK。
桌面 `scripts/build.py --test` 自动打开主应用测试并执行 CTest，再保留六个
独立模块工程的配置、构建和测试；移动端不使用主机 CTest。
