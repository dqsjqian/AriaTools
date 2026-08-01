#include "module/CalendarModule.h"

#include "models/CalendarModel.h"
#include "services/CalendarService.h"
#include "viewmodels/CalendarVm.h"

namespace wb::calendar {

void CalendarModule::ensure_initialized_(wb::module_api::ModuleContext& ctx) {
    if (model_) return;
    service_ = std::make_shared<CalendarService>(
        ctx.services().storage(), ctx.services().http());
    model_ = std::make_shared<CalendarModel>(service_);
}

std::shared_ptr<aria::binding::ViewModel>
CalendarModule::create_view_model(wb::module_api::ModuleContext& ctx) {
    ensure_initialized_(ctx);
    return std::make_shared<CalendarVm>(model_);
}

std::shared_ptr<wb::module_api::IModule> make_calendar_module() {
    return std::make_shared<CalendarModule>();
}

}  // namespace wb::calendar
