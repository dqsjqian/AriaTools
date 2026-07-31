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
- Tools 已完成 `ToolsVm → ToolsModel → ToolsService` 分层调整；ToolsModule 持有模块作用域 Model/Service。
- Tools CTest：Base64、随机字符串、JSON 与加解密往返 1/1 通过。
- Mac 完整 App 构建通过。
- iOS 在 Tools 真实业务接入后构建通过。
- 模块独立构建产物统一放在 `build/<platform>/modules/<module>`。

## 正在进行

### Notes 真实业务

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

1. 新增 Notes 类型与 INotesService/MarkdownNotesService。
2. 新增 NotesModel，并由 NotesModule 创建、持有和注入。
3. NotesVm 改为只依赖 NotesModel。
4. 补齐 Qt/iOS Notes View。
5. 增加 Notes CRUD/持久化业务测试。
6. 运行 Mac 完整构建、Tools/Notes 测试和 iOS 构建。
7. 更新本文件与 `ARCHITECTURE.md` 的实现状态。

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

- 远程仓库已配置为 GitHub 私有仓库，`main` 跟踪 `origin/main`；新设备使用 `git clone --recurse-submodules`。
- 代码与文档不得包含内部工作流、内部系统或公司身份信息。
- 不照搬参考项目中的 Service Locator、全局 Singleton、弱类型 action/event；只吸收其共享 Model 心智模型。
