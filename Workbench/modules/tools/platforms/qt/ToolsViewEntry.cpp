#include "ToolsView.h"
#include "support/QtViewFactory.h"
#include "viewmodels/ToolsVm.h"

namespace wb::tools {

void register_tools_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "tools", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new qtview::ToolsView(static_cast<ToolsVm&>(vm), be);
            return view->widget();
        });
}

}  // namespace wb::tools
