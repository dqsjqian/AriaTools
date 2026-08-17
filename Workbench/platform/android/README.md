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
`scripts/gen-android.sh`（Workbench 下）驱动 NDK + Gradle：
```
scripts/gen-android.sh           # 阶段1：NDK 交叉编译 core 静态库 → build/platforms/android/
scripts/gen-android.sh --apk     # 阶段1 + 阶段2：Gradle assembleDebug 出 APK
scripts/gen-android.sh clean     # 清构建产物
```

阶段 1 等价于：
```
cmake -DWORKBENCH_TARGET_ANDROID=ON -DWORKBENCH_TARGET_QT=OFF \
      -DANDROID_ABI=arm64-v8a \
      -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
```
产出 `build/platforms/android/lib/*.a`（wb_* + aria_*）+ `build/platforms/android/i18n/`。

## 架构（JNI side-channel，同 Aria demo5）
```
C++ AppCore / VM（aria::Property）→ on_changed →
JNI 回调（JniBridge.onPropertyChanged）→ Kotlin StateFlow → Compose 重组
```
业务逻辑全在 C++；Kotlin 只做 StateFlow 薄壳。

## 桥接进度
- [x] AppCore 启动 + 模块导航（6 模块列表）
- [x] dashboard 模块属性桥（welcome / summary 端到端）
- [ ] notes / calendar / tools / settings / sync 属性桥（逐模块接入）
- [ ] 跨线程队列（postToMain + drain，背景线程推 Property 时启用）

## 依赖
- Android NDK r26+（本机 29.0.14206865）、SDK CMake 3.22.1 + ninja
- Gradle 8.2（wrapper 自带）、AGP 8.2.0、Kotlin 1.9.22、Compose BOM 2024.06.00
