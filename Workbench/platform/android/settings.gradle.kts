// platform/android — Workbench Android (JNI + Compose) Gradle project.
// Standalone Gradle project (not part of the main CMake build), mirroring
// the Aria demo5 layout: the NDK cross-build (scripts/gen-android.sh stage 1)
// produces the core static libs, this project links them through the JNI
// bridge in src/main/cpp and renders with Kotlin/Compose.
pluginManagement {
    val profileFile = file("android-dependencies.json")
    check(profileFile.isFile) {
        "Android dependency lock is missing. Run python tools/ci/android_dependencies.py resolve from the repository root."
    }
    val profile = groovy.json.JsonSlurper().parse(profileFile) as Map<*, *>
    val resolved = profile["resolved"] as? Map<*, *> ?: error("Resolve Android dependencies before configuring Gradle")
    val versions = resolved["versions"] as Map<*, *>
    plugins {
        id("com.android.application") version versions["agp"].toString()
        id("org.jetbrains.kotlin.plugin.compose") version versions["kotlin"].toString()
    }
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
