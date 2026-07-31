#include "viewmodels/DashboardVm.h"

namespace wb::dashboard {

DashboardVm::DashboardVm(wb::services::II18nService& i18n)
    : wb::core::LocalizedVm(i18n, "dashboard")
{
    bind_text(welcome, "welcome");
    bind_text(summary, "summary");
}

void DashboardVm::on_activate() {}
void DashboardVm::on_deactivate() { bag().clear(); }

}  // namespace wb::dashboard
