# platform/android —— Android (JNI) 端【预留，后续阶段接入】

## 定位
复用 `core/`（纯 C++，经 NDK r26+ 交叉编译为静态库），View 层用原生
Kotlin/Compose，通过 `aria::adapters::jni` 把 `Property`/`Command` 桥接到
Android View。与 iOS/Qt 完全对称：一份 core，换一套 View + 适配器。

## 计划结构
```
platform/android/
├── app/                     # Gradle app module（Kotlin + Compose）
├── jni/                     # C++ 桥：AppCore ↔ JNI（复用 core/）
│   ├── AndroidShell.cpp
│   └── jni_bindings.cpp
├── CMakeLists.txt           # externalNativeBuild 用
└── build.gradle
```

## 生成方式（规划）
`scripts/gen-android.sh` 将驱动 Gradle + NDK：
```
cmake -DWORKBENCH_TARGET_QT=OFF -DARIA_BUILD_JNI=ON \
      -DANDROID_ABI=arm64-v8a \
      -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
```

## 依赖
- Android NDK r26+
- Gradle
