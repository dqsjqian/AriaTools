#include "infra/log/Log.h"

#include "utils/Platform.h"

#include <cstdio>

#if WB_OS_ANDROID
#  include <android/log.h>
#endif

namespace wb::log {

namespace {
const char* level_tag(Level lv) {
    switch (lv) {
        case Level::Trace: return "T";
        case Level::Debug: return "D";
        case Level::Info:  return "I";
        case Level::Warn:  return "W";
        case Level::Error: return "E";
        case Level::Fatal: return "F";
    }
    return "?";
}
}  // namespace

void init_default_sink() {
    aria::runtime::Logger::instance().set_sink(
        [](Level lv, std::string_view tag, std::string_view msg) {
#if WB_OS_ANDROID
            int prio = ANDROID_LOG_INFO;
            switch (lv) {
                case Level::Trace: case Level::Debug: prio = ANDROID_LOG_DEBUG; break;
                case Level::Info:  prio = ANDROID_LOG_INFO;  break;
                case Level::Warn:  prio = ANDROID_LOG_WARN;  break;
                case Level::Error: case Level::Fatal: prio = ANDROID_LOG_ERROR; break;
            }
            __android_log_print(prio, std::string(tag).c_str(), "%.*s",
                                static_cast<int>(msg.size()), msg.data());
#else
            std::fprintf(stderr, "[%s][%.*s] %.*s\n",
                         level_tag(lv),
                         static_cast<int>(tag.size()), tag.data(),
                         static_cast<int>(msg.size()), msg.data());
#endif
        });
}

}  // namespace wb::log
