# platform/android —— Android (JNI) 端

## 定位
复用 `core/`（纯 C++，经 NDK 交叉编译为静态库），View 层用原生
Kotlin/Compose，通过 JNI side-channel 把 `Property`/`Command` 桥接到
Android。与 iOS/Qt 完全对称：一份 core，换一套 View + 适配器。

## 结构
```
platform/android/
├── jni/                     # C++ 桥：AndroidShell + jni_bridge（复用 core/）
│   ├── AndroidShell.h/cpp   # 持 AppCore，暴露模块元数据 + 激活
│   └── jni_bridge.cpp       # JNI_OnLoad + 属性订阅 → Kotlin 回调
├── app/                     # Gradle app module（Kotlin + Compose + JNI）
│   ├── build.gradle.kts
│   └── src/main/
│       ├── cpp/CMakeLists.txt   # externalNativeBuild：aria_jni.so 链接 core 静态库
│       ├── java/com/dqsjqian/ariatools/
│       │   ├── MainActivity.kt  # assets/i18n → filesDir + 建 shell
│       │   ├── JniBridge.kt     # 静态回调 + native 声明
│       │   ├── AppViewModel.kt  # StateFlow 壳
│       │   └── AppRoot.kt       # Compose 导航 + 页面
│       └── AndroidManifest.xml
├── settings.gradle.kts / build.gradle.kts / gradle.properties / gradlew
└── README.md
```

## 生成方式
在仓库根目录运行 `Workbench/scripts/gen-android.sh`，驱动 NDK + Gradle：
```
bash Workbench/scripts/gen-android.sh           # 阶段1：NDK 交叉编译 core 静态库 → build/platforms/android/
bash Workbench/scripts/gen-android.sh --apk     # 阶段1 + 阶段2：Gradle assembleDebug 出 APK
bash Workbench/scripts/gen-android.sh clean     # 清构建产物
```

阶段 1 等价于：
```
cmake -S Workbench -B build/platforms/android -G Ninja \
      -DWORKBENCH_TARGET_ANDROID=ON -DWORKBENCH_TARGET_QT=OFF \
      -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 \
      -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake"
```
产出 `build/platforms/android/lib/*.a`（wb_* + aria_*）+ `build/platforms/android/i18n/`。

## 架构（JNI side-channel，同 Aria demo5）
```
C++ AppCore / VM（aria::Property）→ on_changed →
JNI 回调（JniBridge.onPropertyChanged）→ Kotlin StateFlow → Compose 重组
```
业务逻辑全在 C++；Kotlin 只做 StateFlow 薄壳。

## 桥接入口
- Compose 页面由 [ModulePages.kt](app/src/main/java/com/dqsjqian/ariatools/ui/ModulePages.kt) 注册；各模块在 `Workbench/modules/<module>/platforms/android/` 提供页面与 JNI 绑定。
- [JniBridge.kt](app/src/main/java/com/dqsjqian/ariatools/JniBridge.kt) 的 `postToMain` 通过主线程 Handler 驱动原生任务队列。
- `ViewBindingLabActivity` 另行验证 View-backed `JniAdapter` 绑定路径。

## 依赖
- Android NDK 29.0.14206865、SDK CMake 3.22.1 + Ninja
- Gradle 8.7（wrapper 自带）、AGP 8.5.2、Kotlin 1.9.22、Compose BOM 2024.06.00
