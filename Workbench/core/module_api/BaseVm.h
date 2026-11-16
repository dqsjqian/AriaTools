#pragma once

// Workbench VM convenience facade. The binding implementation belongs to
// aria::i18n; source-location module inference remains application policy.
#include "aria/binding/view_model.hpp"
#include "aria/i18n/i18n.hpp"
#include "aria/reactive/reactive.hpp"
#include "infra/i18n/I18n.h"

#include <functional>
#include <source_location>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace wb::core {

class BaseVm : public aria::binding::ViewModel {
public:
    explicit BaseVm(wb::i18n::Context context) : context_(std::move(context)) {
        if (!context_) throw std::invalid_argument("BaseVm requires an i18n context");
    }

    [[nodiscard]] aria::i18n::LocaleContext& i18n() const { return *context_; }
    [[nodiscard]] const wb::i18n::Context& i18n_context() const { return context_; }

protected:
    // Existing Workbench localizers run once and when the locale/catalog
    // changes. They can contain side effects (for example rebuilding form
    // rules), so preserve that contract without tracking their other reads.
    void localize(std::function<void()> fn) {
        text_bindings_.push_back(aria::i18n::bind_localized(
            context_, [fn = std::move(fn)] { aria::reactive::untracked(fn); }));
    }

    void text(aria::Property<std::string>& prop, std::string key,
              std::source_location loc = std::source_location::current()) {
        const auto domain = wb::i18n::detail::module_of(loc.file_name());
        text_bindings_.push_back(aria::i18n::bind_text(
            context_, std::string(domain.empty() ? wb::i18n::kGlobalModule : domain),
            prop, std::move(key)));
    }

private:
    wb::i18n::Context context_;
    std::vector<aria::Subscription> text_bindings_;
};

}  // namespace wb::core
