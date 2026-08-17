#pragma once
//
// SearchModule — IModule implementation + factory declaration.
//
#include "module_api/IModule.h"

namespace wb::search {

class SearchModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "search"; }
    std::string nav_key() const override { return "nav_search"; }
    int order() const override { return 10; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;
};

}  // namespace wb::search

namespace wb::module_api { class IModule; }
namespace wb::search {
std::shared_ptr<wb::module_api::IModule> make_search_module();
}
