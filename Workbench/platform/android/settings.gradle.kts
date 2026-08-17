// platform/android — Workbench Android (JNI + Compose) Gradle project.
// Standalone Gradle project (not part of the main CMake build), mirroring
// the Aria demo5 layout: the NDK cross-build (scripts/gen-android.sh stage 1)
// produces the core static libs, this project links them through the JNI
// bridge in src/main/cpp and renders with Kotlin/Compose.
pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "WorkbenchAndroid"
include(":app")
