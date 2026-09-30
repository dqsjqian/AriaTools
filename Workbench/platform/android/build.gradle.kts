// AGP 9 provides Kotlin support. Keep its compiler aligned with Compose.
buildscript {
    val profile = groovy.json.JsonSlurper().parse(file("android-dependencies.json")) as Map<*, *>
    val resolved = profile["resolved"] as? Map<*, *> ?: error("Resolve Android dependencies before configuring Gradle")
    val versions = resolved["versions"] as Map<*, *>
    repositories { google(); mavenCentral() }
    dependencies { classpath("org.jetbrains.kotlin:kotlin-gradle-plugin:${versions["kotlin"]}") }
    configurations.classpath { resolutionStrategy.activateDependencyLocking() }
}

plugins {
    id("com.android.application") apply false
    id("org.jetbrains.kotlin.plugin.compose") apply false
}

val androidProfile = groovy.json.JsonSlurper().parse(file("android-dependencies.json")) as Map<*, *>
val androidResolved = androidProfile["resolved"] as? Map<*, *> ?: error("Resolve Android dependencies before configuring Gradle")
extra["androidDependencyVersions"] = androidResolved["versions"] as Map<*, *>

allprojects {
    dependencyLocking {
        lockAllConfigurations()
        lockMode.set(LockMode.STRICT)
    }
}
