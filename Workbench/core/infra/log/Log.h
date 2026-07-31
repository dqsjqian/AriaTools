#pragma once
//
// Log.h — 应用日志基础模块。薄封装 Aria runtime Logger，提供带 tag 的分级日志 +
// 便捷宏与流式/格式化用法。属 wb_infra（链 aria::runtime）。
//
// 用法：
//   WB_LOGI("notes", "loaded " << count << " notes");   // 流式
//   wb::log::info("sync", "pushed");                     // 直调
//   wb::log::init_default_sink();                        // main 里装默认 stderr sink
//
#include "aria/runtime/logger.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace wb::log {

using Level = aria::runtime::LogLevel;

inline void log(Level lv, std::string_view tag, std::string_view msg) {
    aria::runtime::Logger::instance().log(lv, tag, msg);
}
inline void trace(std::string_view tag, std::string_view m) { log(Level::Trace, tag, m); }
inline void debug(std::string_view tag, std::string_view m) { log(Level::Debug, tag, m); }
inline void info (std::string_view tag, std::string_view m) { log(Level::Info,  tag, m); }
inline void warn (std::string_view tag, std::string_view m) { log(Level::Warn,  tag, m); }
inline void error(std::string_view tag, std::string_view m) { log(Level::Error, tag, m); }

/// 设置最低日志级别（低于此级别不输出）。
inline void set_level(Level lv) { aria::runtime::Logger::instance().set_level(lv); }

/// 安装一个默认 sink：把日志按 "[LEVEL][tag] message" 打到 stderr。
/// 各平台外壳可在 main 里调用（或替换为写文件 / OSLog / logcat）。
void init_default_sink();

}  // namespace wb::log

// ── 便捷流式宏：WB_LOGI("tag", "x=" << x) ──────────────────────────────
#define WB_LOG_STREAM(LEVELFN, TAG, EXPR)                    \
    do {                                                      \
        std::ostringstream _wb_oss;                           \
        _wb_oss << EXPR;                                      \
        ::wb::log::LEVELFN((TAG), _wb_oss.str());             \
    } while (0)

#define WB_LOGT(TAG, EXPR) WB_LOG_STREAM(trace, TAG, EXPR)
#define WB_LOGD(TAG, EXPR) WB_LOG_STREAM(debug, TAG, EXPR)
#define WB_LOGI(TAG, EXPR) WB_LOG_STREAM(info,  TAG, EXPR)
#define WB_LOGW(TAG, EXPR) WB_LOG_STREAM(warn,  TAG, EXPR)
#define WB_LOGE(TAG, EXPR) WB_LOG_STREAM(error, TAG, EXPR)
