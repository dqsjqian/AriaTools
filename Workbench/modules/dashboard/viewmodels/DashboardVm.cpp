#include "viewmodels/DashboardVm.h"

#include "infra/i18n/I18n.h"

namespace wb::dashboard {

DashboardVm::DashboardVm()
{
    text(welcome, "welcome");
    text(summary, "summary");
}

void DashboardVm::on_activate() {}
void DashboardVm::on_deactivate() { bag().clear(); }

}  // namespace wb::dashboard
