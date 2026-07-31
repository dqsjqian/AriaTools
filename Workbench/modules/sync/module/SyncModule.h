#pragma once
#include "module_api/IModule.h"

namespace wb::sync {

class SyncModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "sync"; }
    std::string nav_key() const override { return "nav_sync"; }
    int order() const override { return 5; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

std::shared_ptr<wb::module_api::IModule> make_sync_module();

}  // namespace wb::sync
