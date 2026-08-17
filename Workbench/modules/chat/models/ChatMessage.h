#pragma once
#include <string>

namespace wb::chat {

struct ChatMessage {
    std::string user;
    std::string text;
};

}  // namespace wb::chat
