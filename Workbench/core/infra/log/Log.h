#pragma once
//
// Log.h — 应用日志基础模块。统一行格式（管道分隔，便于 grep / 切列）：
//
//   2026/07/28 21:12:44.171|info|t#3|NotesModel.cpp:113|create_note|saved note xxx
//   └── 日期时间(ms) ──┘ │level│线程│  文件:行     │  函数   │  消息
//
// 推荐用法（流式、自动上下文，无需手写位置）：
//   log_info  << "loaded " << count << " notes";
//   log_error << "GET " << url << " failed: " << err;
//
// 显式 tag（无源位置时，tag 落在「文件」列，行/函数留空）：
//   wb::log::info("sync") << "pushed " << n << " commits";
//   wb::log::info("sync", "pushed");
//
// 输出去向由 set_output 决定；main 里调用 wb::log::init_default_sink()
// 装默认输出（stderr / Android→logcat）。级别用 set_level 过滤，关闭时
// operator<< 短路、不拼接，几乎零开销。
//
#include "aria/runtime/logger.hpp"
#include "utils/Platform.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <functional>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace wb::log {

using Level = aria::runtime::LogLevel;

/// 输出回调：收到的是「已格式化好的完整一行」（不含换行）。
using OutputFn = std::function<void(Level, std::string_view line)>;

/// 设置输出去向（默认未装时不输出）。线程安全由实现保证。
void set_output(OutputFn fn);

/// 安装默认输出：stderr（Android→logcat）。各平台外壳在 main 里调用。
void init_default_sink();

/// 设置最低日志级别（低于此级别不输出）。
inline void set_level(Level lv) { aria::runtime::Logger::instance().set_level(lv); }

/// 级别名（小写，用于行格式）。命名避开 aria::runtime::level_name 以免 ADL 歧义。
constexpr std::string_view level_text(Level lv) {
    switch (lv) {
        case Level::Trace: return "trace";
        case Level::Debug: return "debug";
        case Level::Info:  return "info";
        case Level::Warn:  return "warn";
        case Level::Error: return "error";
        case Level::Fatal: return "fatal";
    }
    return "?";
}

namespace detail {

/// 把一行日志提交给当前 output（内部使用）。定义在 Log.cpp。
void emit(Level lv, std::string_view line);

/// 取源文件短名（去目录）。
constexpr std::string_view short_file(std::string_view path) {
    const auto pos = path.find_last_of("/\\");
    return pos == std::string_view::npos ? path : path.substr(pos + 1);
}

/// 从 source_location 的完整签名里抽「裸函数名」。
/// 例："bool wb::notes::NotesModel::create_note()" → "create_note"。
/// 文件名+行号已能定位，函数名只保留末段以求简洁。
inline std::string short_func(std::string_view sig) {
    // 找参数列表左括号（模板/返回类型里的括号少见，取第一个即可满足绝大多数场景）。
    const auto paren = sig.find('(');
    std::string_view head = (paren == std::string_view::npos) ? sig : sig.substr(0, paren);
    // 去掉尾随空格
    while (!head.empty() && head.back() == ' ') head.remove_suffix(1);
    // 从右往左找作用域/空格分隔，取最后一段作为函数名。
    const auto cut = head.find_last_of(": >");  // 处理 a::b、模板 '>'、返回类型后的空格
    if (cut != std::string_view::npos) head = head.substr(cut + 1);
    return std::string(head);
}

/// 本线程稳定短号，比 std::thread::id 哈希更易读。
inline unsigned thread_seq() {
    static std::atomic<unsigned> counter{0};
    thread_local unsigned id = ++counter;
    return id;
}

/// "YYYY/MM/DD HH:MM:SS.mmm"（本地时钟）。
inline std::string now_datetime_ms() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto t = system_clock::to_time_t(now);
    const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::tm tm{};
#if WB_OS_WINDOWS
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%04d/%02d/%02d %02d:%02d:%02d.%03d",
                  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec, static_cast<int>(ms.count()));
    return std::string(buf);
}

}  // namespace detail

// ── 流式日志：RAII，析构时格式化并提交 ─────────────────────────────────────
class LogStream {
public:
    // 自动上下文版（源位置）
    LogStream(Level lv, const std::source_location& loc)
        : level_(lv),
          active_(lv >= aria::runtime::Logger::instance().level()),
          file_(detail::short_file(loc.file_name())),
          func_(active_ ? detail::short_func(loc.function_name()) : std::string{}),
          line_(loc.line()) {}

    // 显式 tag 版（tag 落在「文件」列，行/函数留空）
    LogStream(Level lv, std::string_view tag)
        : level_(lv),
          active_(lv >= aria::runtime::Logger::instance().level()),
          file_(tag) {}

    ~LogStream() {
        if (!active_) return;
        std::ostringstream line;
        line << detail::now_datetime_ms() << '|'
             << level_text(level_) << '|'
             << 't' << '#' << detail::thread_seq() << '|'
             << file_;
        if (line_ != 0) line << ':' << line_;
        line << '|' << func_ << '|' << oss_.str();
        detail::emit(level_, line.str());
    }

    LogStream(const LogStream&) = delete;
    LogStream& operator=(const LogStream&) = delete;
    LogStream(LogStream&&) = delete;
    LogStream& operator=(LogStream&&) = delete;

    template <typename T>
    LogStream& operator<<(T&& value) {
        if (active_) oss_ << std::forward<T>(value);
        return *this;
    }

private:
    Level level_;
    bool active_;
    std::string file_;   ///< 源文件短名，或显式 tag
    std::string func_;   ///< 函数名（显式 tag 时为空）
    unsigned line_ = 0;  ///< 行号（显式 tag 时为 0，不打印）
    std::ostringstream oss_;
};

// 流式入口（显式 tag，单参）。wb::log::info("tag") << "x=" << x;
inline LogStream trace(std::string_view tag) { return LogStream(Level::Trace, tag); }
inline LogStream debug(std::string_view tag) { return LogStream(Level::Debug, tag); }
inline LogStream info (std::string_view tag) { return LogStream(Level::Info,  tag); }
inline LogStream warn (std::string_view tag) { return LogStream(Level::Warn,  tag); }
inline LogStream error(std::string_view tag) { return LogStream(Level::Error, tag); }

// 直调（两参：tag + 完整消息）。
inline void trace(std::string_view tag, std::string_view m) { LogStream(Level::Trace, tag) << m; }
inline void debug(std::string_view tag, std::string_view m) { LogStream(Level::Debug, tag) << m; }
inline void info (std::string_view tag, std::string_view m) { LogStream(Level::Info,  tag) << m; }
inline void warn (std::string_view tag, std::string_view m) { LogStream(Level::Warn,  tag) << m; }
inline void error(std::string_view tag, std::string_view m) { LogStream(Level::Error, tag) << m; }

// 自动上下文入口：由宏在调用点注入 source_location。
inline LogStream at(Level lv, const std::source_location& loc = std::source_location::current()) {
    return LogStream(lv, loc);
}

}  // namespace wb::log

// ── 自动上下文流式宏：log_info << "x=" << x; ────────────────────────────────
// 用宏是为了在「调用点」抓取 std::source_location（写成函数会抓到函数内部）。
#define log_trace ::wb::log::at(::wb::log::Level::Trace)
#define log_debug ::wb::log::at(::wb::log::Level::Debug)
#define log_info  ::wb::log::at(::wb::log::Level::Info)
#define log_warn  ::wb::log::at(::wb::log::Level::Warn)
#define log_error ::wb::log::at(::wb::log::Level::Error)

// ── 兼容旧宏（保留既有调用点）──────────────────────────────────────────────
#define WB_LOG_STREAM(LEVELFN, TAG, EXPR)  do { ::wb::log::LEVELFN((TAG)) << EXPR; } while (0)
#define WB_LOGT(TAG, EXPR) WB_LOG_STREAM(trace, TAG, EXPR)
#define WB_LOGD(TAG, EXPR) WB_LOG_STREAM(debug, TAG, EXPR)
#define WB_LOGI(TAG, EXPR) WB_LOG_STREAM(info,  TAG, EXPR)
#define WB_LOGW(TAG, EXPR) WB_LOG_STREAM(warn,  TAG, EXPR)
#define WB_LOGE(TAG, EXPR) WB_LOG_STREAM(error, TAG, EXPR)
