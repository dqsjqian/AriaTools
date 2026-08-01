#pragma once
//
// Platform.h — Platform/compiler detection macros and common utilities.
// Cross-platform code should use these instead of scattering #ifdef __APPLE__
// in business code. Part of wb_utils (header-only, zero dependencies).
//
// Provides:
//   OS:       WB_OS_MACOS / WB_OS_IOS / WB_OS_WINDOWS / WB_OS_LINUX / WB_OS_ANDROID
//   Family:   WB_OS_APPLE / WB_OS_POSIX
//   Compiler: WB_COMPILER_CLANG / WB_COMPILER_GCC / WB_COMPILER_MSVC
//   Common:   WB_UNUSED(x) / WB_LIKELY / WB_UNLIKELY / WB_FORCE_INLINE / WB_NODISCARD
//   Name:     wb::platform_name()
//

// ── Operating system ────────────────────────────────────────────────────
#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #define WB_OS_APPLE 1
    #if TARGET_OS_IPHONE
        #define WB_OS_IOS 1
        #define WB_PLATFORM_NAME "ios"
    #else
        #define WB_OS_MACOS 1
        #define WB_PLATFORM_NAME "macos"
    #endif
#elif defined(_WIN32) || defined(_WIN64)
    #define WB_OS_WINDOWS 1
    #define WB_PLATFORM_NAME "windows"
#elif defined(__ANDROID__)
    #define WB_OS_ANDROID 1
    #define WB_PLATFORM_NAME "android"
#elif defined(__linux__)
    #define WB_OS_LINUX 1
    #define WB_PLATFORM_NAME "linux"
#else
    #define WB_PLATFORM_NAME "unknown"
#endif

#if defined(WB_OS_MACOS) || defined(WB_OS_IOS) || defined(WB_OS_LINUX) || defined(WB_OS_ANDROID)
    #define WB_OS_POSIX 1
#endif

// Undefined values default to 0, so #if works uniformly.
#ifndef WB_OS_MACOS
    #define WB_OS_MACOS 0
#endif
#ifndef WB_OS_IOS
    #define WB_OS_IOS 0
#endif
#ifndef WB_OS_WINDOWS
    #define WB_OS_WINDOWS 0
#endif
#ifndef WB_OS_LINUX
    #define WB_OS_LINUX 0
#endif
#ifndef WB_OS_ANDROID
    #define WB_OS_ANDROID 0
#endif
#ifndef WB_OS_APPLE
    #define WB_OS_APPLE 0
#endif
#ifndef WB_OS_POSIX
    #define WB_OS_POSIX 0
#endif

// ── Compiler ────────────────────────────────────────────────────────────
#if defined(__clang__)
    #define WB_COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define WB_COMPILER_GCC 1
#elif defined(_MSC_VER)
    #define WB_COMPILER_MSVC 1
#endif

// ── Common macros ───────────────────────────────────────────────────────
#define WB_UNUSED(x) ((void)(x))

#if defined(WB_COMPILER_CLANG) || defined(WB_COMPILER_GCC)
    #define WB_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define WB_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #define WB_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define WB_LIKELY(x)   (x)
    #define WB_UNLIKELY(x) (x)
    #define WB_FORCE_INLINE inline
#endif

namespace wb {
/// Runtime platform name (matches WB_PLATFORM_NAME).
inline const char* platform_name() { return WB_PLATFORM_NAME; }
}  // namespace wb
