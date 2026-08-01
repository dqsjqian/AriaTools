#pragma once
//
// DashboardVm — home overview. Text is dispatched by the VM via i18n (View has zero string literals).
//
#include "aria/aria.hpp"
#include "module_api/BaseVm.h"

namespace wb::dashboard {

class DashboardVm final : public wb::core::BaseVm {
public:
    DashboardVm();

    aria::Property<std::string> welcome;   ///< Welcome text (updates on language change)
    aria::Property<std::string> summary;   ///< Summary text (updates on language change)

    void on_activate() override;
    void on_deactivate() override;
};

}  // namespace wb::dashboard
