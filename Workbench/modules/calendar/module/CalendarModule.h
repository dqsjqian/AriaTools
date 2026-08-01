#pragma once
#include "module_api/IModule.h"

namespace wb::calendar {

class CalendarService;
class CalendarModel;

class CalendarModule final : public wb::module_api::IModule {
public:
    std::string id() const override { return "calendar"; }
    std::string nav_key() const override { return "nav_calendar"; }
    int order() const override { return 2; }
    std::shared_ptr<aria::binding::ViewModel>
        create_view_model(wb::module_api::ModuleContext& ctx) override;

private:
    void ensure_initialized_(wb::module_api::ModuleContext& ctx);

    std::shared_ptr<CalendarService> service_;
    std::shared_ptr<CalendarModel> model_;
};

std::shared_ptr<wb::module_api::IModule> make_calendar_module();

}  // namespace wb::calendar
