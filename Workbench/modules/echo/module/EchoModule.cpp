#include "module/EchoModule.h"
#include "viewmodels/EchoVm.h"
namespace wb::echo {
std::shared_ptr<aria::binding::ViewModel>
EchoModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    auto vm = std::make_shared<EchoVm>(ctx.i18n_context());
    vm->message.set("Echo module — hot-pluggable!");
    return vm;
}
std::shared_ptr<wb::module_api::IModule> make_echo_module() {
    return std::make_shared<EchoModule>();
}
}  // namespace wb::echo
