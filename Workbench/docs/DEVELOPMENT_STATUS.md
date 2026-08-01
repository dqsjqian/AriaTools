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
- i18n 重构：全局门面 `wb::i18n::str("key")`（模块由源文件自动推断，回退 common，不跨模块）；`BaseVm` 提供 `text()/localize()` 自动随语言刷新，删除旧 `LocalizedVm`。
- 全局流式日志 `log_info << ...`：管道格式 `日期 时间(ms)|level|线程|文件:行|函数|消息`。
- 新增跨平台 `IHttpClient` 基础设施服务（当前 Stub，真实 curl+mbedTLS 待接）。
- Notes 真实业务：Markdown 文件持久化 + NotesModel/Service + Qt/iOS 视图 + CTest。
- Calendar：月视图网格 + .ics 订阅（CalendarModel/Service，事件解析），CTest 覆盖解析/排布/抓取。
- 远端仓库配置从 Settings 迁入 Sync（同步中心自包含）；Settings 保留应用级偏好（当前：界面语言）。
- Aria 适配器补齐按钮文案：qt6(QAbstractButton/QComboBox)、uikit(UIButton)、appkit(NSButton/NSPopUpButton)。
- Windows 桌面端构建跑通（Qt6 + MSVC，默认 Ninja 并行编译，绕开 MSBuild MSB4166）；gen-win.ps1 自动探测 VS/SDK/Qt6 并补齐 rc.exe/mt.exe 的 PATH。
- 源码英文化重构：Workbench 下 .h/.cpp/.mm 注释与字符串、CMakeLists.txt、构建脚本全部改为英文（语言选择器里的语言本名字面量如"简体中文"保留）。

## 正在进行

### Notes 真实业务

已完成的结构：

```text
modules/notes/
├── models/                # Note 数据类型 + NotesModel 共享业务状态
├── services/              # INotesService + MarkdownNotesService
├── viewmodels/            # NotesVm（只依赖 NotesModel）
├── platforms/qt|ios/      # 列表-编辑器视图
├── module/                # NotesModule 装配 Service/Model 并注入 VM
├── assets/i18n/           # 模块文案
└── tests/                 # CTest 业务验证
```

已落地的业务：

- 一条笔记一个 `notes/<id>.md`，front matter 保存 id/title/updated_at，正文为 Markdown。
- NotesModel 持有 `ObservableList<Note>` 与选中/草稿状态，暴露 reload/新建/选择/编辑/保存/删除/导入附件。
- 保存与新建成功经 EventBus 发布强类型 `wb::events::NoteSaved`。
- 附件复制到 `notes/assets/<id>/` 并生成相对 Markdown 路径；删除笔记连带清理附件目录。
- Qt：左侧列表 + 状态 + 新建，右侧标题/正文编辑与保存/删除；iOS：等价 VM 绑定（标题/正文编辑 + 增删改）。
- NotesModule 以模块作用域创建并持有 MarkdownNotesService/NotesModel，构造函数注入 VM。

### 验证结果（本轮）

- Notes CTest：往返（新建→保存→重载→读取）、front matter 解析、附件导入相对路径、删除清理、NoteSaved 事件，全部通过（1/1）。
- Tools CTest：1/1 通过。
- Mac 完整 App 构建通过（含 Notes Qt 视图）。
- iOS 构建：见下方验证命令重跑确认。

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

```powershell
# Windows 完整 App
powershell -File Workbench/scripts/gen-win.ps1
```

## 仓库注意事项

- 远程仓库已配置为 GitHub 私有仓库，`main` 跟踪 `origin/main`；新设备使用 `git clone --recurse-submodules`。
- 代码与文档不得包含内部工作流、内部系统或公司身份信息。
- 不照搬参考项目中的 Service Locator、全局 Singleton、弱类型 action/event；只吸收其共享 Model 心智模型。
