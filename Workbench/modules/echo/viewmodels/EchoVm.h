#pragma once
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
namespace wb::echo {
class EchoVm : public wb::core::BaseVm {
public:
    EchoVm() { text(title, "title"); text(message, "message"); }
    aria::Property<std::string> title;
    aria::Property<std::string> message;
};
}  // namespace wb::echo
