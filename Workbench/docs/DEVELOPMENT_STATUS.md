# Workbench 开发状态

> 跨会话交接入口。每完成一个实现或验证里程碑后同步更新。

## 当前目标

将 Workbench 建设为 Aria C++ MVVM 的跨平台标杆案例。当前阶段先统一模块内分层，再完成 Notes 真实业务。

## 已确认架构

详细规则见 `ARCHITECTURE.md` 的“模块内 MVVM 分层”。核心调用链：

```text
View → ViewModel → Module Model → Module Service → Core Infrastructure
```

Model 和 Service 是装配树中的作用域实例，不是静态全局 Singleton。模块内多个 VM 共享一个 Model；依赖均通过构造函数注入。

## 已完成

- Workbench 六模块骨架以及 Qt/macOS、UIKit/iOS 外壳。
- 稳定基础设施接口：i18n、storage、settings、sync、secret、crypto。
- Tools：Base64、随机字符串、JSON 格式化/压缩、Apple 原生文件加解密。
- Tools 独立业务 harness：JSON 与加解密往返已通过。
- Mac 完整 App 构建通过。
- iOS 在 Tools 真实业务接入后构建通过。
- 模块独立构建产物统一放在 `build/<platform>/modules/<module>`。

## 正在进行

### 1. Tools 分层调整

把当前：

```text
ToolsVm → ToolsService
```

调整为：

```text
ToolsVm → ToolsModel → ToolsService
```

- ToolsModule 持有一份 ToolsService 和一份共享 ToolsModel。
- ToolsVm 只调用/订阅 ToolsModel。
- 保持现有功能行为不变。

### 2. Notes 真实业务

目标结构：

```text
modules/notes/
├── models/                # Note 数据类型 + NotesModel 共享业务状态
├── services/              # INotesService + MarkdownNotesService
├── viewmodels/
├── platforms/qt|ios/
├── module/
├── assets/i18n/           # 模块文案；icons 等资源也归 assets/
└── tests/                 # CTest 模块业务验证
```

业务范围：

- 一条笔记一个 `notes/<uuid>.md` 文件。
- Markdown front matter 保存 id/title/updatedAt。
- 列表加载、新建、选择、标题/正文编辑、保存、删除。
- 附件复制到笔记专属目录并生成相对 Markdown 引用。
- 保存成功发布强类型 `NoteSaved` 事件。
- Qt 提供列表、Markdown 源编辑与预览；iOS 保持相同 VM/Model 能力。

## 后续顺序

1. 完成 ToolsModel 重构并运行 Tools tests。
2. 新增 Notes 类型与 INotesService/MarkdownNotesService。
3. 新增 NotesModel，并由 NotesModule 创建、持有和注入。
4. NotesVm 改为只依赖 NotesModel。
5. 补齐 Qt/iOS Notes View。
6. 增加 Notes CRUD/持久化业务测试。
7. 运行 Mac 完整构建、Tools/Notes 测试和 iOS 构建。
8. 更新本文件与 `ARCHITECTURE.md` 的实现状态。

## 验证命令

```bash
# Mac 完整 App
bash Workbench/scripts/gen-mac.sh

# iOS
bash Workbench/scripts/gen-ios.sh build

# Tools 模块测试
cmake -S Workbench/modules/tools/tests -B build/mac/modules/tools
cmake --build build/mac/modules/tools
ctest --test-dir build/mac/modules/tools --output-on-failure
```

## 仓库注意事项

- 仓库已建立 Git 管理；`third_party/aria` 以 submodule 固定版本，构建产物与工作流运行态不入库。
- 不推送、不切换分支，除非用户明确要求。
- 不照搬参考项目中的 Service Locator、全局 Singleton、弱类型 action/event；只吸收其共享 Model 心智模型。
