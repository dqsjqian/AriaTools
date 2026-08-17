#pragma once
//
// TipcalcModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::tipcalc {

class TipcalcModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "tipcalc"; }
    std::string nav_key() const override { return "nav_tipcalc"; }
    int order() const override { return 6; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::tipcalc

namespace wb::module_api { class IModule; }
namespace wb::tipcalc {
std::shared_ptr<wb::module_api::IModule> make_tipcalc_module();
}
