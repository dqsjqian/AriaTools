#pragma once
//
// Log.h — Application logging foundation. Uniform line format (pipe-delimited, easy to grep / column-split):
//
//   2026/07/28 21:12:44.171|info|t#3|NotesModel.cpp:113|create_note|saved note xxx
//   └── datetime(ms) ──┘ │level│thread│  file:line    │ function │ message
//
// Recommended usage (streaming, auto-context, no need to write location manually):
//   log_info  << "loaded " << count << " notes";
//   log_error << "GET " << url << " failed: " << err;
//
// Explicit tag (when no source location; tag goes in the "file" column; line/function empty):
//   wb::log::info("sync") << "pushed " << n << " commits";
//   wb::log::info("sync", "pushed");
//
// Output destination is set by set_output; call wb::log::init_default_sink()
// in main to install the default output (stderr / Android->logcat). Filter by
// level via set_level; when disabled, operator<< short-circuits and does no
// formatting — near-zero overhead.
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

/// Output callback: receives a fully formatted complete line (without newline).
using OutputFn = std::function<void(Level, std::string_view line)>;

/// Set the output destination (default: no output if not installed). Thread safety guaranteed by the implementation.
void set_output(OutputFn fn);

/// Install the default output: stderr (Android->logcat). Each platform shell calls this in main.
void init_default_sink();

/// Set the minimum log level (levels below this are not output).
inline void set_level(Level lv) { aria::runtime::Logger::instance().set_level(lv); }

/// Level name (lowercase, for the line format). Named to avoid ADL ambiguity with aria::runtime::level_name.
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

/// Submit a log line to the current output (internal use). Defined in Log.cpp.
void emit(Level lv, std::string_view line);

/// Get the short source file name (without directory).
constexpr std::string_view short_file(std::string_view path) {
    const auto pos = path.find_last_of("/\\");
    return pos == std::string_view::npos ? path : path.substr(pos + 1);
}

/// Extract the "bare function name" from source_location's full signature.
/// E.g. "bool wb::notes::NotesModel::create_note()" -> "create_note".
/// File name + line number already locate the code; function name keeps only
/// the last segment for brevity.
inline std::string short_func(std::string_view sig) {
    // Find the opening paren of the parameter list (parens in templates/return
    // types are rare; taking the first one works for the vast majority of cases).
    const auto paren = sig.find('(');
    std::string_view head = (paren == std::string_view::npos) ? sig : sig.substr(0, paren);
    // Strip trailing spaces
    while (!head.empty() && head.back() == ' ') head.remove_suffix(1);
    // Scan right-to-left for scope/space separators; take the last segment as the function name.
    const auto cut = head.find_last_of(": >");  // handles a::b, template '>', space after return type
    if (cut != std::string_view::npos) head = head.substr(cut + 1);
    return std::string(head);
}

/// A stable short ID for this thread; more readable than std::thread::id hash.
inline unsigned thread_seq() {
    static std::atomic<unsigned> counter{0};
    thread_local unsigned id = ++counter;
    return id;
}

/// "YYYY/MM/DD HH:MM:SS.mmm" (local clock).
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

// ── Streaming logging: RAII, formats and commits on destruction ──────────
class LogStream {
public:
    // Auto-context version (source location)
    LogStream(Level lv, const std::source_location& loc)
        : level_(lv),
          active_(lv >= aria::runtime::Logger::instance().level()),
          file_(detail::short_file(loc.file_name())),
          func_(active_ ? detail::short_func(loc.function_name()) : std::string{}),
          line_(loc.line()) {}

    // Explicit tag version (tag goes in the "file" column; line/function empty)
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
    std::string file_;   ///< Short source file name, or explicit tag
    std::string func_;   ///< Function name (empty for explicit tag)
    unsigned line_ = 0;  ///< Line number (0 for explicit tag; not printed)
    std::ostringstream oss_;
};

// Streaming entry (explicit tag, single arg). wb::log::info("tag") << "x=" << x;
inline LogStream trace(std::string_view tag) { return LogStream(Level::Trace, tag); }
inline LogStream debug(std::string_view tag) { return LogStream(Level::Debug, tag); }
inline LogStream info (std::string_view tag) { return LogStream(Level::Info,  tag); }
inline LogStream warn (std::string_view tag) { return LogStream(Level::Warn,  tag); }
inline LogStream error(std::string_view tag) { return LogStream(Level::Error, tag); }

// Direct call (two args: tag + full message).
inline void trace(std::string_view tag, std::string_view m) { LogStream(Level::Trace, tag) << m; }
inline void debug(std::string_view tag, std::string_view m) { LogStream(Level::Debug, tag) << m; }
inline void info (std::string_view tag, std::string_view m) { LogStream(Level::Info,  tag) << m; }
inline void warn (std::string_view tag, std::string_view m) { LogStream(Level::Warn,  tag) << m; }
inline void error(std::string_view tag, std::string_view m) { LogStream(Level::Error, tag) << m; }

// Auto-context entry: macro injects source_location at the call site.
inline LogStream at(Level lv, const std::source_location& loc = std::source_location::current()) {
    return LogStream(lv, loc);
}

}  // namespace wb::log

// ── Auto-context streaming macros: log_info << "x=" << x; ────────────────
// A macro is used to capture std::source_location at the call site (a function would capture its own internals).
#define log_trace ::wb::log::at(::wb::log::Level::Trace)
#define log_debug ::wb::log::at(::wb::log::Level::Debug)
#define log_info  ::wb::log::at(::wb::log::Level::Info)
#define log_warn  ::wb::log::at(::wb::log::Level::Warn)
#define log_error ::wb::log::at(::wb::log::Level::Error)

// ── Legacy compatibility macros (kept for existing call sites) ───────────
#define WB_LOG_STREAM(LEVELFN, TAG, EXPR)  do { ::wb::log::LEVELFN((TAG)) << EXPR; } while (0)
#define WB_LOGT(TAG, EXPR) WB_LOG_STREAM(trace, TAG, EXPR)
#define WB_LOGD(TAG, EXPR) WB_LOG_STREAM(debug, TAG, EXPR)
#define WB_LOGI(TAG, EXPR) WB_LOG_STREAM(info,  TAG, EXPR)
#define WB_LOGW(TAG, EXPR) WB_LOG_STREAM(warn,  TAG, EXPR)
#define WB_LOGE(TAG, EXPR) WB_LOG_STREAM(error, TAG, EXPR)
