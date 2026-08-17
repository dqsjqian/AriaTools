#include "ToolsView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/ToolsVm.h"

namespace wb::tools {

void register_tools_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "tools", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto view = std::make_shared<iosview::ToolsView>(static_cast<ToolsVm&>(vm), be);
            UIViewController* vc = view->viewController();
            wb::ios::ui::attach_owner(vc, std::move(view));
            return vc;
        });
}

}  // namespace wb::tools
