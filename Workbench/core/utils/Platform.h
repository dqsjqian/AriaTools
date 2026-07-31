#pragma once
//
// Platform.h — 平台/编译器识别宏与通用宏。跨端代码统一用这里，
// 不在业务代码里散落 #ifdef __APPLE__ 等。属 wb_utils（纯头、零依赖）。
//
// 提供：
//   操作系统：WB_OS_MACOS / WB_OS_IOS / WB_OS_WINDOWS / WB_OS_LINUX / WB_OS_ANDROID
//   家族：    WB_OS_APPLE / WB_OS_POSIX
//   编译器：  WB_COMPILER_CLANG / WB_COMPILER_GCC / WB_COMPILER_MSVC
//   通用：    WB_UNUSED(x) / WB_LIKELY / WB_UNLIKELY / WB_FORCE_INLINE / WB_NODISCARD
//   平台名：  wb::platform_name()
//

// ── 操作系统 ────────────────────────────────────────────────────────────
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

// 未定义的置 0，便于统一用 #if。
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

// ── 编译器 ──────────────────────────────────────────────────────────────
#if defined(__clang__)
    #define WB_COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define WB_COMPILER_GCC 1
#elif defined(_MSC_VER)
    #define WB_COMPILER_MSVC 1
#endif

// ── 通用宏 ──────────────────────────────────────────────────────────────
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
/// 运行时可取的平台名（与 WB_PLATFORM_NAME 一致）。
inline const char* platform_name() { return WB_PLATFORM_NAME; }
}  // namespace wb
