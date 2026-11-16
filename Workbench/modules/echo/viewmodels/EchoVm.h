#pragma once
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
namespace wb::echo {
class EchoVm : public wb::core::BaseVm {
public:
    EchoVm(wb::i18n::Context context) : BaseVm(std::move(context)) { text(title, "title"); text(message, "message"); }
    aria::Property<std::string> title;
    aria::Property<std::string> message;
};
}  // namespace wb::echo
