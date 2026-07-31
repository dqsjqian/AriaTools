#include "module/CalendarModule.h"
#include "viewmodels/CalendarVm.h"

namespace wb::calendar {

std::shared_ptr<aria::binding::ViewModel>
CalendarModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    return std::make_shared<CalendarVm>(ctx.services().i18n());
}

std::shared_ptr<wb::module_api::IModule> make_calendar_module() {
    return std::make_shared<CalendarModule>();
}

}  // namespace wb::calendar
