#include "infra/log/Log.h"

#include "utils/Platform.h"

#include <cstdio>
#include <mutex>

#if WB_OS_ANDROID
#  include <android/log.h>
#endif

namespace wb::log {

namespace {
std::mutex g_mutex;
OutputFn   g_output;  // 未设置时不输出
}  // namespace

void set_output(OutputFn fn) {
    std::lock_guard<std::mutex> lk(g_mutex);
    g_output = std::move(fn);
}

namespace detail {
void emit(Level lv, std::string_view line) {
    std::lock_guard<std::mutex> lk(g_mutex);
    if (g_output) g_output(lv, line);
}
}  // namespace detail

void init_default_sink() {
    set_output([](Level lv, std::string_view line) {
#if WB_OS_ANDROID
        int prio = ANDROID_LOG_INFO;
        switch (lv) {
            case Level::Trace: case Level::Debug: prio = ANDROID_LOG_DEBUG; break;
            case Level::Info:  prio = ANDROID_LOG_INFO;  break;
            case Level::Warn:  prio = ANDROID_LOG_WARN;  break;
            case Level::Error: case Level::Fatal: prio = ANDROID_LOG_ERROR; break;
        }
        __android_log_print(prio, "workbench", "%.*s",
                            static_cast<int>(line.size()), line.data());
#else
        (void)lv;
        std::fprintf(stderr, "%.*s\n",
                     static_cast<int>(line.size()), line.data());
#endif
    });
}

}  // namespace wb::log
