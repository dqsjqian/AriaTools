// app — Workbench Android app module (Kotlin/Compose + JNI bridge).
//
// Links the NDK cross-built core static libs (scripts/gen-android.sh stage 1)
// via src/main/cpp/CMakeLists.txt. Paths are passed from the command line:
//   ./gradlew assembleDebug -PwbNativeRoot=<AriaTools repo root>
plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.plugin.compose")
}

val dependencyVersions = rootProject.extra["androidDependencyVersions"] as Map<*, *>
fun dependencyVersion(name: String) = (dependencyVersions[name] ?: error("Missing Android dependency version: $name")).toString()

// AriaTools repository root (contains Workbench/ and build/deps/aria).
val wbNativeRoot: String = providers.gradleProperty("wbNativeRoot")
    .orElse("../../..")   // app → platform/android → Workbench → AriaTools root
    .get()

android {
    namespace = "com.dqsjqian.ariatools"
    compileSdk = dependencyVersion("compile_sdk").toInt()
    buildToolsVersion = dependencyVersion("build_tools")
    ndkVersion = dependencyVersion("ndk")

    defaultConfig {
        applicationId = "com.dqsjqian.ariatools"
        minSdk = 24
        targetSdk = 34
        versionCode = 4
        versionName = "1.1.0"

        ndk {
            abiFilters += listOf("arm64-v8a")
        }

        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++23"
                arguments += listOf(
                    "-DWB_NATIVE_ROOT=$wbNativeRoot"
                )
            }
        }
    }

    sourceSets["main"].assets.directories.add("$wbNativeRoot/build/platforms/android/i18n")

    // Module Android views live beside the C++ module sources (one Compose
    // page per module in platforms/android/), mirroring QT_SOURCES /
    // IOS_SOURCES — the Android twin of the per-platform View layout.
    sourceSets["main"].kotlin.directories.addAll(listOf(
        "$wbNativeRoot/Workbench/modules/dashboard/platforms/android",
        "$wbNativeRoot/Workbench/modules/echo/platforms/android",
        "$wbNativeRoot/Workbench/modules/frameworklab/platforms/android",
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
    ))

    buildFeatures {
        compose = true
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = dependencyVersion("cmake")
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.toVersion(dependencyVersion("java"))
        targetCompatibility = JavaVersion.toVersion(dependencyVersion("java"))
    }

}


// Preserve original notices from the exact native SDK and resolved runtime jars.
val licenseAssets = layout.buildDirectory.dir("generated/thirdPartyAssets")
val runtimeLicenseConfigurations = providers.provider {
    listOf("debugRuntimeClasspath", "releaseRuntimeClasspath").map { configurations.getByName(it) }
}
val runtimeLicenseArtifacts = files(runtimeLicenseConfigurations)
val selectedNdk = androidComponents.sdkComponents.sdkDirectory.map {
    it.dir("ndk/${dependencyVersion("ndk")}").asFile
}
val nativeRootForNotices = file(wbNativeRoot)
val noticesPython = providers.environmentVariable("PYTHON").orElse(
    if (System.getProperty("os.name").startsWith("Windows")) "python" else "python3"
)
val stageThirdPartyNotices = tasks.register<Exec>("stageThirdPartyNotices") {
    inputs.files(runtimeLicenseArtifacts)
    outputs.dir(licenseAssets)
    // Native source overrides can change independently of the Gradle graph.
    outputs.upToDateWhen { false }
    doFirst {
        val components = runtimeLicenseConfigurations.get().flatMap { configuration ->
            configuration.incoming.resolutionResult.allComponents.mapNotNull { component ->
                val id = component.id as? org.gradle.api.artifacts.component.ModuleComponentIdentifier
                id?.let { "${it.group}:${it.module}:${it.version}=${configuration.name}" }
            }
        }.sorted()
        commandLine(listOf(
            noticesPython.get(), nativeRootForNotices.resolve("tools/ci/stage_android_notices.py").path,
            "--native-root", nativeRootForNotices.path,
            "--ndk", selectedNdk.get().path,
            "--output", licenseAssets.get().asFile.path
        ) + runtimeLicenseArtifacts.files.sortedBy { it.name }.flatMap { listOf("--artifact", it.path) }
          + components.flatMap { listOf("--runtime-component", it) })
    }
}
android.sourceSets["main"].assets.directories.add(licenseAssets.get().asFile.path)
tasks.named("preBuild").configure { dependsOn(stageThirdPartyNotices) }

dependencies {
    val composeBom = platform("androidx.compose:compose-bom:${dependencyVersion("compose_bom")}")
    implementation(composeBom)
    implementation("androidx.compose.ui:ui")
    implementation("androidx.compose.material3:material3")
    implementation("androidx.compose.foundation:foundation")
    implementation("androidx.compose.ui:ui-tooling-preview")
    implementation("androidx.activity:activity-compose:${dependencyVersion("activity")}")
    implementation("androidx.lifecycle:lifecycle-viewmodel-compose:${dependencyVersion("lifecycle")}")
    implementation("androidx.lifecycle:lifecycle-runtime-compose:${dependencyVersion("lifecycle")}")

    debugImplementation("androidx.compose.ui:ui-tooling")
}
