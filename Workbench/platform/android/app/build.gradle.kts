// app — Workbench Android app module (Kotlin/Compose + JNI bridge).
//
// Links the NDK cross-built core static libs (scripts/gen-android.sh stage 1)
// via src/main/cpp/CMakeLists.txt. Paths are passed from the command line:
//   ./gradlew assembleDebug -PwbNativeRoot=<AriaTools repo root>
plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

// AriaTools repository root (contains Workbench/ and third_party/aria).
val wbNativeRoot: String = providers.gradleProperty("wbNativeRoot")
    .orElse("../../..")   // app → platform/android → Workbench → AriaTools root
    .get()

android {
    namespace = "com.dqsjqian.ariatools"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.dqsjqian.ariatools"
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        ndk {
            abiFilters += listOf("arm64-v8a")
        }
        ndkVersion = "29.0.14206865"

        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++20"
                arguments += listOf(
                    "-DWB_NATIVE_ROOT=$wbNativeRoot"
                )
            }
        }
    }

    sourceSets["main"].assets.srcDirs("$wbNativeRoot/build/platforms/android/i18n")

    // Module Android views live beside the C++ module sources (one Compose
    // page per module in platforms/android/), mirroring QT_SOURCES /
    // IOS_SOURCES — the Android twin of the per-platform View layout.
    sourceSets["main"].java.srcDirs(
        "$wbNativeRoot/Workbench/modules/dashboard/platforms/android",
        "$wbNativeRoot/Workbench/modules/echo/platforms/android",
        "$wbNativeRoot/Workbench/modules/notes/platforms/android",
        "$wbNativeRoot/Workbench/modules/calendar/platforms/android",
        "$wbNativeRoot/Workbench/modules/tools/platforms/android",
        "$wbNativeRoot/Workbench/modules/settings/platforms/android",
        "$wbNativeRoot/Workbench/modules/sync/platforms/android",
        "$wbNativeRoot/Workbench/modules/tipcalc/platforms/android",
        "$wbNativeRoot/Workbench/modules/unitconvert/platforms/android",
        "$wbNativeRoot/Workbench/modules/cart/platforms/android",
        "$wbNativeRoot/Workbench/modules/signup/platforms/android",
        "$wbNativeRoot/Workbench/modules/search/platforms/android",
        "$wbNativeRoot/Workbench/modules/login/platforms/android",
        "$wbNativeRoot/Workbench/modules/chat/platforms/android",
        "$wbNativeRoot/Workbench/modules/theme/platforms/android",
        "$wbNativeRoot/Workbench/modules/wizard/platforms/android",
    )

    buildFeatures {
        compose = true
    }

    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.8"
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    kotlinOptions {
        jvmTarget = "17"
    }
}

dependencies {
    val composeBom = platform("androidx.compose:compose-bom:2024.06.00")
    implementation(composeBom)
    implementation("androidx.compose.ui:ui")
    implementation("androidx.compose.material3:material3")
    implementation("androidx.compose.foundation:foundation")
    implementation("androidx.compose.ui:ui-tooling-preview")
    implementation("androidx.activity:activity-compose:1.8.2")
    implementation("androidx.lifecycle:lifecycle-viewmodel-compose:2.7.0")
    implementation("androidx.lifecycle:lifecycle-runtime-compose:2.7.0")

    debugImplementation("androidx.compose.ui:ui-tooling")
}
