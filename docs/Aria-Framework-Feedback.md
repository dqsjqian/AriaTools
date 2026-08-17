# Aria 框架使用反馈

**—— 基于 AriaTools 15 模块 × 4 平台实战，写给 Aria 维护者的评估建议**

> 作者：AriaTools 业务应用开发者（基于 Aria v1.1.0）
> 日期：2026-08-17
> 范围：Qt6 桌面 / iOS UIKit / Android Compose+JNI / 多语言 i18n / 模块化插件化

---

## 一、背景

我用 Aria 框架从零构建了 **AriaTools**：一个 16 个业务模块（dashboard / notes / calendar / tools / settings / sync / tipcalc / unitconvert / cart / signup / search / login / chat / theme / wizard / echo）的跨平台示例应用，每模块一套 VM + Model + Service，Qt / iOS / Android 三端 View 壳，中英双语 i18n，模块热插拔。

这份文档是我作为**业务应用开发者**（而非框架作者）的真实体会：哪些地方让开发顺畅、哪些地方卡了我很久、哪些是我被迫在业务层打补丁的。每条都给出具体建议，供维护者评估取舍。

---

## 二、先肯定：Aria 的内核是对的

1. **VM 层声明式体验极佳**——`Property / Computed / Command` 配合响应式图自动依赖追踪，写业务逻辑不需要手写 notify/observer。15 个模块的 VM 全部零平台依赖，一份 C++ 代码跑三端，这是最核心的价值。
2. **生命周期级联**——`ViewModel::add_child()` + activate/deactivate 级联，chat 的 Publisher/Subscriber 子 VM 随父 VM 自动激活/停用，订阅自动清理，这个设计非常优雅。
3. **编译期类型安全**——绑定类型错误直接编译失败，不会运行时崩。
4. **adapter 抽象正确**——`IView` 类型擦除 + 平台 adapter，让 BindingEngine 不依赖任何平台类型，这是跨端统一的关键。

> **Aria 没有 BaseVm、没有 `text()` 自动 i18n 机制**——这些是 **AriaTools 项目自己封装的**（见 `core/module_api/BaseVm.h`），不是 Aria 框架的一部分。原反馈文档（早期版本）误把 BaseVm::text() 列为"框架优点"，这里**严格修正**：Aria 框架本身没有 i18n 自动刷新能力，开发者要自己在 Aria 之上封装。

---

## 三、模块化 / 插件化：编译期解耦做到了，运行时动态还缺

### 3.1 现状

- 每模块一个静态库（`wb_module_<name>.a`），`wb_add_module()` 单行声明
- CMake 用 `file(GLOB)` 自动扫描 `modules/*/CMakeLists.txt`，生成 `GeneratedModuleList.h`，`ModulesManifest.cpp` 和 Qt/iOS `ViewManifest` 用宏展开自动注册
- **热插拔验证通过**：新增模块只加目录、删除模块只删目录，核心文件零改动，编译不报错
- 模块间通信走 `EventBus`（进程级单例），cart 模块 publish `ItemAddedToCart`，dashboard / chat / notes 三个模块的 VM 同时收到——**无直接耦合**，已验证

### 3.2 痛点

1. **没有真正的运行时动态加载**。目前是编译期静态链接 + 自动发现，做不到 dlopen/运行时装卸。对"插件市场""运行时升级模块"这类场景（Aria 的 `plugin-property-demo` 展示了 ABI 插件能力）缺一个标准的 `IModuleLoader` 入口，业务层想用还得自己造轮子。
2. **模块间隐式耦合点残留**。删掉一个模块后，虽然编译通过，但这些地方可能残留脏引用：
   - Android `JniBridge` / `AndroidShell` 的 `subscribe_all` / `set_text` / `execute_command` 是按模块 id 硬路由的（`if (id == "sync") ...`）——删模块必须同步改这个文件，**违背了"只删目录"的承诺**
   - `_shared/events/CrossModuleEvents.h` 里的事件定义是全局共享的，模块删了事件还在
   - i18n 资源是按模块目录收集的（这个没问题，但 View 里直接 `wb::i18n::str_in("mod","key")` 的调用没有编译期校验，key 拼错不报错）
3. **新增模块的样板代码仍偏多**：CMakeLists + Module.h/.cpp + VM.h/.cpp + 3 平台 View + i18n XML × 2 +（Android 还要改 ModulePages.kt / JniBridge 路由）。虽然核心文件不用改，但**没有一个脚手架工具**。

### 3.3 建议

- **P1**：提供 `IModuleLoader`（参考 plugin-property-demo 的 ABI 方案），让模块可编译为 `.so/.dylib/.dll` 动态加载，热插拔从"编译期"升级为"运行时"
- **P1**：把 Android 的 JNI 路由（subscribe_all / set_text / execute_command）改成**注册表驱动**而非 if-else 硬路由，和 Qt/iOS 的 ViewManifest 一样自动发现
- **P2**：提供脚手架 CLI（`wb new-module <name>`），生成完整模块模板
- **P2**：模块依赖显式声明（`MODULE_DEPS`），编译期检测循环依赖

---

## 四、View ↔ VM 绑定：自由度是双刃剑，API 覆盖不全是大坑

### 4.1 最疼的坑：`Computed<T>` 不能进绑定 API

```cpp
// ❌ 编译失败！bind_text_projected 只接受 Property<T>&，不接受 Computed<T>
be.bind_text_projected(vm.tipAmount, view_for(label), [](const double& v){...});

// ✅ 被迫退化为手写 on_changed + 手动管理订阅袋
auto& subs = wb::ios::ui::subs_keepalive();   // 这是我 Hack 的全局订阅袋
subs.push_back(vm.tipAmount.on_changed([label](const double v){ ... }));
```

`Computed` 是最常用的响应式值，但绑定 API 不覆盖它。tipcalc（3 个 Computed）、unitconvert（3 个 Computed）都踩了。**对新人这是第一个劝退坑**。

### 4.2 enum Property 绑定要靠绕路

```cpp
aria::Property<Category> category{Category::Temperature};  // enum

// ❌ adapter 的 set_int/get_int 只支持 int，enum 不能直接绑 UIStepper/ComboBox
// ✅ 被迫在 VM 里加 3 个 Command：
aria::Command<> selectTemperature{[this]{ category.set(Category::Temperature); }};
aria::Command<> selectLength{...};
aria::Command<> selectWeight{...};
```

一个分类选择器要写 3 个 Command 才能让三端统一驱动——认知负担明显。

### 4.3 平台订阅生命周期不对称（iOS 无 subs_attached_to）

- Qt 有 `subs_attached_to(QObject*)`——订阅挂到 widget 上，widget 销毁自动断开
- iOS **没有等价物**——我被迫 Hack 了一个**进程级全局订阅袋** `subs_keepalive()`：

```cpp
std::vector<aria::Subscription>& subs_keepalive() {
    static std::vector<aria::Subscription> v;
    return v;  // 进程存活期间永不释放，只适合 demo，正式 App 是泄漏
}
```

这是最让我不舒服的地方：**同一个框架，两端的订阅生命周期管理能力不对称**。iOS View 一旦要绑 Computed 或手动订阅，就没有干净的生命周期归属。

### 4.4 bind API 太多太杂，命名不统一

实际用到的绑定入口：`bind_text / bind_text_oneway / bind_text_converted / bind_text_projected / bind_optional_text / bind_double / bind_int / bind_bool / bind_command / bind_visible` ——

- `oneway` 后缀 vs 无后缀（双向）的含义靠猜
- `converted`（带 Converter）vs `projected`（带投影函数）语义接近但不同
- 没有一份 API 清单/速查表，全靠读源码

### 4.5 "绑定过于自由"——谦哥的原话，我认同

View 可以绑任何 Property 的任何方向（VM→View / View→VM / 双向），没有任何约束。好处是灵活；坏处是：
- 新人不知道"该绑哪个方向"（oneway vs 双向）
- 同一个 Property 被多个 View 绑定/解绑的时机难追踪
- 没有"约定式"的绑定 schema，团队协作时各自为政

### 4.6 建议

- **P0**：`binding_engine` 为 `Computed<T>` 增加绑定重载（或提供 `to_property(computed)` 适配器）——这是最高频需求
- **P0**：iOS 提供 per-Controller 的订阅袋（等价于 Qt 的 `subs_attached_to`），消灭全局 keepalive Hack
- **P1**：提供 `enum → int` 的绑定适配（Converter 支持枚举，或 Property<Enum> 自动转 int）
- **P1**：写一份绑定 API 速查表（README 或 docs），标注每个 API 的语义、方向、适用类型
- **P2**：设计"约定式绑定"（如 VM 声明 `BINDINGS` 元数据，View 按声明绑定），降低自由度的认知成本

---

## 五、i18n 多语言：框架没内建能力，业务项目自封装

### 5.1 现状

**Aria 框架本身没有 i18n 能力**。`Property<std::string>` 只是一个 string 容器，文本翻译要自己用 `tr("module", "key")` 之类的函数查表。

`BaseVm::text()` 和 `localize()` 是 **AriaTools 项目自封装的**（`core/module_api/BaseVm.h`），做的事：
- `text(prop, key)` — 绑定 Property 到 i18n key（用 `std::source_location` 从调用点文件名自动推断 module id）
- `localize(fn)` — 注册语言切换时重跑的闭包
- 自动订阅 `wb::i18n::on_language_changed`，所有 localizer 在切语言时自动重跑

这套封装确实好用——比手写"每加一个 property 都要在 language change handler 里手动 set"干净得多。**但这是我们自己的补丁，不是 Aria 框架的**。如果 Aria 框架内建这个能力，业务层不用各自造。

### 5.2 痛点（即使有了 BaseVm 封装）

1. **非 BaseVm 的模块享受不到**。`UnitConvertVm` 是普通类（继承 `ViewModel` 而不是 `BaseVm`），被迫用 `UnitConvertVmHostVm` 包一层才能进 IModule 契约——i18n 也得手动在 HostVm 里 `text()` 一遍。**两种 VM 写法并存，新人会困惑"我该继承谁"**。
2. **Android 端无法直接调 i18n**（JNI 桥接限制），View 渲染 label 只能靠 VM 暴露 Property。结果 CartVm 被迫加了 9 个 label Property（nameLabel/priceLabel/addLabel/countLabel/subtotalLabel/taxLabel/totalLabel/...），全部只是为了喂给 Android View：
   ```cpp
   // CartVm.h —— 纯为了 Android 渲染 label 而加的一堆冗余 Property
   aria::Property<std::string> nameLabel;
   aria::Property<std::string> priceLabel;
   ...
   ```
   这在 Qt/iOS 端根本用不上（它们直接 `wb::i18n::str_in()`）。**业务 VM 被平台能力短板污染了**。
3. i18n key 无编译期校验，拼错**显示丑陋的 fallback**（`[common/key]`）——而不是空串或错误。echo 模块的 `nav_echo` 就被误放到了 echo 模块的 i18n（应该放 common，因为 `nav_title()` 只查 common），结果 UI 显示 `[common/nav_echo]`，调试时容易困惑。

### 5.3 建议

- **P0**：把 i18n 能力内建到 Aria 框架（参考 BaseVm::text 模式）——ViewModel 派生类直接获得 `text()` / `localize()`，并通过 `SourceLocation` 自动推断 module id
- **P1**：i18n 提供 C 导出（`extern "C"`）+ JNI 直读，让 Android View 能直接取文案，VM 不用为平台加冗余 label Property
- **P2**：i18n key 编译期生成枚举或宏，拼错直接编译失败
- **P2**：i18n fallback 机制改进——key 找不到时，框架应明确报错（log + 抛异常）而不是静默返回 `[common/key]`（开发期容易漏看）

---

## 五点五、Android Kotlin View ↔ C++ VM 对接：Aria 最大的短板

**这是整个框架里让我最难受的一块。** Aria 的 `demo5-android-jni-mvvm` 只演示了单模块、单方向的 C++ → Kotlin 推送；真实业务要三端对齐时，Android 端几乎是我**从零手搓的**——Aria 在 Android 侧几乎没有提供可复用的绑定能力。

### 5.5.1 现状：jni_bridge.cpp 500+ 行手写样板

AriaTools 的 Android 桥接（`platform/android/jni/jni_bridge.cpp` 509 行 + `AndroidShell.cpp` 212 行）全是手写的：

```cpp
// ① 每模块手写 subscribe_all（C++ → Kotlin 推送）
static void subscribe_all(wb::core::AppCore& core) {
    for (auto& entry : core.modules()) {
        if (id == "cart") {
            auto& vm = static_cast<wb::cart::CartVm&>(*entry.vm);
            bind_str(id, "title", vm.title);
            bind_dbl(id, "subtotal", vm.subtotal);
            // ... 每个属性一行，15 模块 ≈ 150 行
        }
    }
}

// ② Kotlin → C++ 文本回写，if-else 硬路由
void AndroidShell::set_text(...) {
    if (moduleId == "notes") { if (propName == "editTitle") n.editTitle.set(value); }
    else if (moduleId == "cart") { if (propName == "draftName") c.draftName.set(value); }
    // ... 15 模块 if-else
}
```

**每个新模块都要手写三处路由**（subscribe_all / set_text / execute_command），且是 if-else 硬编码——**违背了模块热插拔的承诺**（删模块必须同步改这个文件）。

### 5.5.2 类型系统在 JNI 边界彻底崩塌

Qt 端 `be.bind_double(vm.bill, view_for(billSpin))` 是类型安全的；Android 端一切退化为 **String 传输**：

```kotlin
// Kotlin 侧：所有属性变成 Map<String, String>
private val _props = MutableStateFlow<Map<String, String>>(emptyMap())
val props: StateFlow<Map<String, String>> = _props.asStateFlow()
```

- `Property<double>` → `std::to_string` → `"3.50"` → Kotlin `String` → 再 `toDoubleOrNull()`
- `Property<int>` → `"25"` → `String` → 再 `toIntOrNull()`
- `Property<bool>` → `"1"/"0"` → `String` → 再 `== "1"`
- **列表（ObservableList）更离谱**：用 `"\n"` 拼接字符串传输，Kotlin 再 `split('\n')` 还原——没有类型、没有 item 结构、纯文本 hack
- 命令调用是字符串：`execute("cart", "addItem")`——**拼错静默无操作**，没有编译期检查

**对比**：Qt/iOS 用强类型绑定，Android 用字符串协议。同一份 VM 逻辑，三端绑定体验天差地别。

### 5.5.3 生命周期不对称

- C++ VM 的 activate/deactivate 由 `AndroidShell::activate_module` 驱动，但 **Compose 页面没有 Controller 生命周期概念**——VM 激活时机和 Compose recomposition 生命周期完全脱节
- Qt 有 `subs_attached_to(widget)`，iOS 我 Hack 了 `subs_keepalive()`，Android **根本没有订阅生命周期管理**——订阅要么全局泄漏，要么全手动

### 5.5.4 VM 被 Android 能力短板污染

因为 Android View 无法直接调 i18n（JNI 限制），业务 VM 被迫暴露一堆**只为 Android 渲染 label 的冗余 Property**（第五节 5.2 详述）——业务代码被平台短板反向污染。

### 5.5.5 建议

| 优先级 | 建议 | 说明 |
|---|---|---|
| **P0** | **提供 Android 绑定引擎**（等价 Qt 的 BindingEngine + AndroidAdapter） | JNI 侧提供类型化的 `bind_double/bind_int/bind_bool/bind_text`，而不是 String 协议；Kotlin 侧提供 `StateFlow` 直接映射，消灭手写字符串解析 |
| **P0** | **列表模型绑定**（等价 Qt 的 `qt_list_model_adapter`） | ObservableList → RecyclerView/LazyColumn 原生适配，而不是 `"\n"` 拼接 |
| **P1** | **命令类型化** | `executeCommand(moduleId, cmdName)` 改为编译期生成的命令枚举/接口 |
| **P1** | **订阅生命周期** | JNI 侧提供 per-Activity/per-Fragment 的 SubscriptionBag（等价 Qt `subs_attached_to`） |
| **P1** | **JNI 路由注册表化** | subscribe_all/set_text/execute_command 改为注册表驱动，配合热插拔 |
| **P2** | **demo5 升级** | 从单模块单向推送，升级为多模块 + 双向 + 类型化的完整示例 |

**一句话**：Aria 在 Qt/iOS 是"成熟的绑定框架"，在 Android 是"裸 JNI 基础"——中间差了一整个适配层。业务开发者要用它做 Android 端，等于自己把框架该做的活全干了。

---

## 五点六、工程集成陷阱：Aria 假设与实际工程约束的冲突

调试 macOS app 启动时连续踩了 4 个工程集成坑——这些是 **Aria 框架假设与实际多模块项目约束不匹配** 的真实案例，建议维护者考虑提供更明确的"集成检查表"或文档化"启动时序约束"。

### 5.6.1 陷阱 1：`GeneratedModuleList.h` 用 `#pragma once` 导致宏不重展开

CMake 自动发现模块并生成 `GeneratedModuleList.h`（每个模块一行 `WB_MODULE_ENTRY(mod, Mod)`）。`ModulesManifest.cpp` 想用它做两件事：
- Phase 1：forward-declare 各模块的 `make_xxx_module()`
- Phase 2：调各模块的 `registry.add(make_xxx_module())`

**问题**：第二次 `#include "app/GeneratedModuleList.h"` 因为 `#pragma once` 整个文件被跳过——**宏根本不展开**。结果 `populate_modules` 注册了 0 个模块，nav 列表空、stack 空。

**症状**：macOS app 启动后 UI 完全空。诊断 `core.modules().size() == 0`。

**修复**：CMake 生成时**不写 `#pragma once`**（让宏每次 include 都重展开）。但 Aria 框架本身如果用 X-Macro 模式生成模块注册表，这个坑会复现——建议**文档化"自动生成头不要 #pragma once"**或者提供官方 helper 宏。

### 5.6.2 陷阱 2：ServiceHub 注入时序与 AppCore 构造耦合

`AppCore` 构造函数里**直接** `populate_modules + create_view_model`（同步创建所有 VM）。但 `AsyncCommand` VM（如 login）构造时要求 `IExecutor&` 不能是 `InlineExecutor`（graph thread-affinity）——**必须**在 VM 创建前注入真线程 executor。

**问题**：`AppCore` 不知道有"先注入 executor 再构造 VM"的时序要求；平台壳（QtAppShell）也不知道需要在 AppCore 构造完**立刻**注入。

**症状**：`std::invalid_argument: AsyncCommand: cannot use InlineExecutor as the graph-thread executor when worker runs on a different thread.`

**修复**：把 `populate_modules` 从构造函数移到显式 `load_modules()` 方法；AppCore 加 `set_ui_executor()` / `set_timer()` 转发到 ServiceHub；平台壳在 AppCore 构造后**先注入**再 `load_modules()`。

**建议**：
- Aria 的 `AppCore`（或等价组装类）应**明确文档化**："构造不会自动创建 VM，需要先注入 platform executors 再调 load_modules()"
- 或框架提供一个"标准启动协议"类（如 `CoreBootstrap`）封装这一套时序

### 5.6.3 陷阱 3：`AsyncCommand` 错误信息不友好

错误 `cannot use InlineExecutor as the graph-thread executor when worker runs on a different thread` 提到了"graph-thread executor"和"worker"——但**没明确告诉开发者要做什么**。第一次看这错误会困惑 5 分钟。

**建议**：
- 错误信息直接给出修复方向：例如 `Pass the UI thread executor (e.g. main-thread IExecutor wrapped from QtDispatcher) to set_ui_executor() before creating AsyncCommand VMs`
- 提供 std::source_location 或 context 注明错误触发位置

### 5.6.4 陷阱 4：`QtDispatcher` 不是 `IExecutor`

`QtDispatcher` 继承 `IDispatcher`（post/post_delayed），但 ServiceHub `set_ui_executor()` 收 `IExecutor*`——需要**自己写 wrapper**（`DispatcherExec` + `DispatcherDelay`）才能注入。

Aria 的 `demo1-qt-showcase` 有这个 wrapper（`Executors.h`），但 Aria 框架本身**没提供**——每个 Qt 业务项目都要自己写一遍（或者从 demo 复制）。

**建议**：把 `DispatcherExec` / `DispatcherDelay` 提到 Aria Qt adapter 里作为官方 helper（命名空间 `aria::adapters::qt6`），demo1 也能简化。

### 5.6.5 陷阱 5：i18n 资源 POST_BUILD 拷贝——改文案不触发，bundle 陈旧

macOS app 的 i18n 资源通过 `add_custom_command(TARGET workbench POST_BUILD copy_directory)` 拷进 `.app` bundle。**POST_BUILD 只在 workbench 二进制重链时执行**——如果只改了 `strings.xml`（内容变了但二进制没变），bundle 里的资源保持旧版，UI 继续显示 `[module/key]` fallback。

**症状**：改完 i18n 重启 app，文案不变（或仍是 key 原文）。要 clean 重建才生效，非常坑。

**修复**：把 bundle staging 改成独立的 `ALL` custom target（`add_custom_target(wb_stage_bundle ALL ...)` + `add_dependencies(wb_stage_bundle workbench)`），每次 build 都重新拷贝，不依赖重链。

**建议**：Aria 文档应提醒"运行时资源（i18n/assets）不要用 POST_BUILD 拷贝，用独立 ALL target 或 install(CODE) 依赖文件变更"。

### 5.6.6 建议汇总

- **P0**：在 Aria 文档加"集成时序检查表"（ServiceHub 注入 → AppCore 构造 → load_modules → 注册 View），每个平台 demo 都遵守
- **P0**：错误信息改进——AsyncCommand / ObservableList 等常见错误给出修复方向
- **P1**：Qt adapter 提供 `DispatcherExec` / `DispatcherDelay` 官方 helper
- **P1**：文档化"自动生成头不要 #pragma once"（X-Macro 模式陷阱）
- **P1**：文档化"运行时资源不要 POST_BUILD 拷贝"（i18n 文案修改不触发，bundle 陈旧）

---

## 六、基于框架的二次封装：哪些本该是框架内建的

作为业务开发者，我被迫在业务层打了这些补丁（都源自框架能力缺失）：

| 我的封装 | 弥补的缺口 | 应该归位 |
|---|---|---|
| `IosUi::view_for` / `make_stack_vc` / `make_label`... | iOS 控件创建 + IView 包装的样板 | 框架内建 UIKit 控件工厂 |
| `IosUi::subs_keepalive()` | iOS 无 per-View 订阅袋（4.3） | 框架内建 per-Controller 订阅袋 |
| `UiHelpers::bind_editable_text` | 文本双向绑定的"只回写不反向刷新"场景 | 框架内建 `bind_text` 变体 |
| `Converter<double,string>` 工厂（三处重复定义） | 数值↔字符串转换的标准写法 | 框架内建常用 Converter |
| `HostVm` 包装（5 个模块） | 普通类 VM 进不了 IModule 契约 | 框架统一 VM 基类 |
| CMake 自动发现 + GeneratedModuleList | 模块注册硬编码问题 | 框架提供标准模块发现机制 |

**判断**：如果这些能力内建到框架（或至少有一份官方样板目录），业务层就不用各自造轮子。AriaTools 的 `platform/ios/support/` 和 `platform/qt/support/` 现在承担了"框架半成品补丁"的角色。

---

## 七、构建与工具链

1. 三平台构建脚本（gen-mac.sh / gen-ios.sh / gen-win.ps1 / gen-android.sh）能跑通，但：
   - iOS 需要 `xcode-select` 切换（Xcode-beta 与 CommandLineTools），脚本里临时用 `DEVELOPER_DIR` 绕过——对 CI 不友好
   - Android JNI 桥接是**全手写**（jni_bridge.cpp 一个文件 400+ 行，按模块 if-else 路由），工作量大且易错，`Aria demo5` 只演示了单模块
2. **建议 P2**：提供统一构建 CLI（`wb build --platform qt|ios|android`），自动探测工具链（vswhere / xcode-select / SDK 路径）

---

## 八、文档：最大的痛

我所有 API 知识都来自**读 Aria 源码**（grep `bind_` 看签名、看 demo1~5 猜用法）。没有任何 API 文档、没有核心概念教程。具体缺失：

- 绑定 API 清单（4.4）
- 生命周期语义（activate/deactivate/child 级联何时触发）
- EventBus 使用模式（demo1 chat 有，但没有文档说明"跨模块通信的标准姿势"）
- i18n 接入指南（text() / str() / str_in() 的区别与场景）
- "写一个新模块"的端到端教程

**建议 P0**：doxygen + 每核心概念一页教程。否则每个新人都要重踩 AriaTools 踩过的坑。

---

## 九、优先级汇总

| 优先级 | 项 | 价值 |
|---|---|---|
| **P0** | `Computed<T>` 绑定重载 | 最高频坑，新人劝退点 |
| **P0** | iOS per-Controller 订阅袋 | 消灭全局 keepalive Hack，正式 App 必需 |
| **P0** | **Android 绑定引擎**（类型化 JNI，消灭 String 协议） | Android 端最大短板，差一个适配层 |
| **P0** | **Android 列表模型绑定** | ObservableList → LazyColumn，而非 "\n" 拼接 |
| **P0** | **i18n 能力内建到 Aria**（参考 BaseVm::text） | 业务层不再各自造轮子 |
| **P0** | **集成时序检查表 + AsyncCommand 错误信息改进** | 4 个真实工程陷阱（#2585、ServiceHub 时序、错误信息、Dispatcher 包装） |
| **P0** | API 文档 + 绑定速查表 | 降低学习曲线 |
| **P1** | 统一 VM 基类（消灭 HostVm） | 消灭双轨制 |
| **P1** | enum 绑定适配 | 减少绕路 |
| **P1** | i18n C 导出（Android 直读） | 业务 VM 不再被平台污染 |
| **P1** | Android JNI 路由注册表化 | 热插拔承诺在 Android 兑现 |
| **P1** | Android 订阅生命周期 / 命令类型化 | 对齐 Qt/iOS 体验 |
| **P1** | Qt adapter 提供 `DispatcherExec` / `DispatcherDelay` | 消除 Qt 项目必写的 wrapper |
| **P1** | 文档化 X-Macro 模式 #pragma once 陷阱 | 防重犯 |
| **P2** | 动态模块加载（IModuleLoader） | 运行时热插拔 |
| **P2** | 脚手架 CLI / 统一构建 CLI | 开发者体验 |
| **P2** | demo5 升级为多模块双向类型化示例 | 官方样板对齐业务需求 |

---

## 十、结语

Aria 的**内核设计是对的**（响应式图 + 类型安全 + 生命周期级联 + adapter 抽象），它让我能用一份 C++ 代码跑三端，这是 Qt/原生/iOS 单独做都做不到的。但**"框架"到"产品"之间还差最后一公里**：绑定 API 覆盖不全、平台订阅生命周期不对称、**Android 端缺整个绑定适配层**、文档缺位、模块系统只有编译期解耦。

如果维护者能优先解决 P0 五项（Computed 绑定、iOS 订阅袋、Android 绑定引擎、Android 列表绑定、文档），Aria 从"优秀框架"到"开发者友好框架"的跨越就完成了——尤其 Android 端，那是投入产出比最高的一块。

—— 期待 Aria 变得更好。有任何需要我补充细节或提供复现代码的，随时联系。
