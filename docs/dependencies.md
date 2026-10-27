# 依赖更新使用指南

本文的命令均从 **AriaTools 仓库根目录**执行。需要 Python 3.14+；macOS/Linux 如果没有 `python` 命令，替换为 `python3`。先在构建使用的 Python 环境中安装 `python -m pip install -r requirements-build.txt`（推荐虚拟环境）。解析、校验和 Git 获取由共享的 ForgeDeps 提供，仓库脚本保留原命令入口。首次解析或主动更新需要联网；GitHub API 可使用已登录的 `gh`，未安装时使用公开 API。构建还需要 README 所列的编译器、CMake 和平台 SDK。

## 单一依赖文件

| 文件 | 用途 | 是否提交 Git |
| --- | --- | --- |
| `dependencies.json` | 每项包含来源、可选长期 `version` 要求，以及工具维护的 `resolved` 版本、完整提交和 SHA256；不填 `version` 表示更新时选最新稳定版 | 是 |
| `scripts/ci/update_dependencies.py` | 手动更新入口，无需修改脚本 | 是 |

已有锁时，普通构建复用锁；没有匹配锁时才解析最新稳定版。**“最新”指执行解析/更新时的最新稳定发布，不是每次构建自动升级，也不包含预发布或开发分支。** 显式版本可选择真实标签对应的预发布或维护补丁版本。已安装 Qt/编译器/系统 SDK 仍由平台工具配置，详见下文。

可用依赖名：`aria`, `doctest`, `json`, `mira`, `openssl`。名称区分大小写，必须与 manifest 中的键一致。

## 常用命令

```bash
# 查看所有参数
python scripts/ci/update_dependencies.py --help

# 按 manifest 要求更新全部依赖
python scripts/ci/update_dependencies.py

# 只更新一个依赖，其他锁记录完全保留
python scripts/ci/update_dependencies.py --only mira

# 更新多个依赖：每个名称使用一个 --only
python scripts/ci/update_dependencies.py --only json --only mira

# 本次指定两个版本；其他库按 manifest（未填 version 的选择最新稳定版）
python scripts/ci/update_dependencies.py --version json=3.12.0 --version openssl=4.0.3

# 只处理指定的两个库，并给其中一个指定版本
python scripts/ci/update_dependencies.py --only json --only mira --version json=3.12.0
```

`--version` 可重复，但同一个名称不能重复。`--only` 与 `--version` 同用时，每个版本覆盖项都必须出现在 `--only` 中；拼错名称或遗漏选择会报错，不会默默忽略。版本写上游稳定版本号，例如 JSON 的 `3.12.0`；不接受 `main`、`nightly`、`2.0.0-rc1` 作为稳定选择。

## 两个库固定、另一个跟最新

无需改 Python 脚本。在现有 manifest 的 `json`、`openssl` 项中分别添加 `"version": "3.12.0"`、`"version": "4.0.3"`，保留其余来源字段；`mira` 项不要加 `version`。例如下面仅是 `json` 条目的写法，**不要用这个片段覆盖整个文件**：

```json
"json": {
  "provider": "github",
  "repo": "nlohmann/json",
  "artifact": "release-asset",
  "asset": "json.tar.xz",
  "tag_prefix": "v",
  "version": "3.12.0"
}
```

然后运行 `python scripts/ci/update_dependencies.py`：两个固定库保持所需版本，未固定库选择最新稳定版。只想升级第三个库时，运行 `python scripts/ci/update_dependencies.py --only mira`。以后想解除固定，删除该项的 `version` 字段再主动更新。

优先级与持久性：

1. 本次 `--version NAME=VERSION` 高于 manifest 的 `version`。
2. manifest 明确版本约束长期生效，后续更新也遵守。
3. 没有显式版本时，普通解析复用有效锁；主动更新才重新选择最新稳定版。
4. 命令行选中的结果会留在锁中。manifest 没有不同的显式版本时，后续普通解析继续复用；manifest 若指定了另一版本，下一次不传覆盖参数的解析会恢复 manifest 的要求。要长期固定，请修改 manifest；本次参数不会改写其中的长期要求。

## 构建与共享 SDK

安装构建工具后，注册一个可供所有项目共享的工作区：

```bash
python -m pip install -r requirements-build.txt
python -m forgedeps init /path/to/ForgeDepsWorkspace
python scripts/build.py --test
```

默认从 `dependencies.json` 的已解析锁准备源码和 SDK；构建不自动选择新版本。`FORGEDEPS_HOME` 可覆盖已注册工作区。源码位于工作区 `sources/`，Aria 和第三方产物位于 `build/<name>/<target>/`；应用直接链接此处，每个目标只保留当前产物。版本、编译器、ABI、源码或配方变化会触发显式构建替换，失败保留旧产物。

`--aria-root /path/to/Aria`（CMake 的 `ARIA_DIR`，Tutorial 为 `ARIA_ROOT`）提供可编辑源码；其内容仍被哈希并编译进共享 SDK，不再嵌入应用子目录。Qt、NDK 和 Apple SDK 由用户安装，实际工具链与 Qt 内容进入产物身份。更新版本使用 `scripts/ci/update_dependencies.py`，审查并提交锁文件。

Python 入口持有配置、编译、测试和部署所需的消费锁。手工命令使用 `python -m forgedeps.build_kit.session --configure -- cmake -S ... -B ...`，后续构建/测试使用 `python -m forgedeps.build_kit.session --build-dir <build> -- <command>`。离线构建要求已准备对应源码、归档和工具链；不兼容或被改动的产物会失败。

`fetch_aria.py` 只用于单独准备共享源码；本地镜像开发 checkout 使用 `--source <repo> --destination <path>`。默认应用构建不需要先运行此命令。

## 失败处理与回退

| 现象 | 处理 |
| --- | --- |
| 未声明的名称、重复参数、选择范围不一致 | 查看 manifest 的键以及 `--help`，修正参数 |
| 版本标签不存在、版本格式错误、下载/API 失败 | 核对上游发布或标签；检查网络或 `gh` 登录/限流；成功前原锁保持不变 |
| 锁内容损坏、SHA256 不符、缓存源码有修改 | 不会跳过验证；先保留本地修改，再恢复可信锁/缓存或使用新的缓存目录 |
| 元数据更新成功，但编译/测试失败 | 新版本可能不兼容；修改集成代码，或恢复已验证的 `dependencies.json`后重新构建 |
| 本地 Aria checkout 有修改 | 先提交或另存修改，获取器不会直接覆盖 |

回退时，从自己已知通过测试的 Git 提交恢复 `dependencies.json`（先保留未提交的修改），然后重复上面的获取、构建、测试流程。恢复依赖文件不会自动恢复已编译二进制。不要手改锁中的哈希来绕过校验。

删除某项的 `resolved` 也会触发重新解析，并丢失该项现有选择。**正常升级请用更新脚本**；不要删除整个 `dependencies.json`，它还保存依赖来源与版本要求。

## 高级参数

| 参数 | 用途 |
| --- | --- |
| `--only NAME` | 只更新某项，可重复 |
| `--version NAME=VERSION` | 本次版本覆盖，可重复 |
| `--file PATH` | 使用另一份同时包含版本要求与解析结果的依赖文件 |
| `--cache-dir PATH` | 解析时下载的校验缓存目录 |
| `--offline` | 禁止联网解析；主动更新通常无法在此模式下完成 |

使用自定义依赖文件进行 Aria 源码构建时，传入 `-DARIA_DEPENDENCIES_FILE=...`；获取和更新工具也传入同一路径的 `--file`。CMake 的临时覆盖保存在构建目录，源码中的单一依赖文件保持不变。

## Android 的独立兼容 profile

上面的 `update_dependencies.py` 更新 C++ 依赖，不更新 Android 的 Maven 图，也不要求 Android SDK。Android 使用单独入口：

```bash
# 普通构建复用已验证锁；缺锁或请求/profile 变化时解析并构建候选组合
python scripts/ci/android_dependencies.py resolve
# 主动更新 Compose BOM、Activity、Lifecycle，并验证完整工具链组合
python scripts/ci/android_dependencies.py update
# 显式版本优先；成功后保存在 Android manifest，后续普通构建仍使用它
python scripts/ci/android_dependencies.py update --version activity=1.13.0
# 不联网、不要求 SDK 的锁完整性检查
python scripts/ci/android_dependencies.py check
```

Android 只维护一个自定义依赖文件 `Workbench/platform/android/android-dependencies.json`：包含版本请求、工具链兼容组，以及 `resolved` 实际版本和验证摘要；请求与兼容组用指纹关联，不再复制到另一份自定义锁。库默认选择解析时的最新稳定版，AGP/Gradle/Kotlin/SDK/NDK 按经过审查的 profile 整组选择，不逐项漂移。`--profile` 高于 `ARIA_ANDROID_PROFILE` 和 manifest；`--version NAME=VERSION` 高于 `ARIA_DEP_ANDROID_<NAME>_VERSION` 和 manifest。当前 profile 使用 AGP 9.4 / Gradle 9.6 / Kotlin 2.4.20 / JDK 17 / compile SDK 37 / NDK r30（30.0.16248370）。

更新需要先安装 profile 对应的 SDK，并运行 `bash Workbench/scripts/gen-android.sh` 生成匹配 NDK 的原生库。设置 `ANDROID_SDK_ROOT`、`JAVA_HOME` 选择已有 SDK/JDK；脚本不会安装全局工具链。候选依赖必须使用全新的 Gradle 用户缓存在临时工程完成 Debug、Release APK 构建，再使用这份缓存进行离线严格校验重放，成功后才保存 profile 锁、Gradle 原生依赖锁、校验元数据与 Gradle wrapper SHA256。并发更新会被拒绝，失败或中断会保留旧锁。检查并提交这一 Android 依赖文件与 Gradle 原生锁、校验文件的变更；后两者是 Gradle 消费完整传递依赖图所需的格式，不是第二份自定义版本清单。具体文件清单、覆盖字段和官方兼容依据见 [Android 依赖说明](android-dependencies.md)。
