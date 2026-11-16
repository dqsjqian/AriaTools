#pragma once

// Workbench keeps its service name as a compatibility alias. Catalog loading,
// fallback, locale state and subscriptions are owned by Aria's i18n module.
#include "aria/i18n/i18n.hpp"

namespace wb::services {
using II18nService = aria::i18n::LocaleContext;
}  // namespace wb::services
