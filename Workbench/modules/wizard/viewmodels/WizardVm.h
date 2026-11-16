#pragma once
//
// WizardVm — Tab 9: 3-step signup wizard (Navigator)
//

#include "aria/aria.hpp"
#include "aria/binding/navigation.hpp"
#include "aria/binding/view_model.hpp"
#include "aria/command.hpp"
#include "module_api/BaseVm.h"

#include <memory>
#include <string>

namespace wb::wizard {

struct WizardDraft {
    aria::Property<std::string> username{""};
    aria::Property<std::string> email{""};
    aria::Property<std::string> theme{"Light"};
};

class Step1Vm : public wb::core::BaseVm {
public:
    std::shared_ptr<WizardDraft> draft;
    explicit Step1Vm(wb::i18n::Context context, std::shared_ptr<WizardDraft> d);
};

class Step2Vm : public wb::core::BaseVm {
public:
    std::shared_ptr<WizardDraft> draft;
    /// Theme picker commands — each sets draft->theme to a fixed value.
    aria::Command<> pickLight    {[this]{ draft->theme.set("Light");     }};
    aria::Command<> pickDark     {[this]{ draft->theme.set("Dark");      }};
    aria::Command<> pickSolarized{[this]{ draft->theme.set("Solarized"); }};
    explicit Step2Vm(wb::i18n::Context context, std::shared_ptr<WizardDraft> d);
};

class Step3Vm : public wb::core::BaseVm {
public:
    std::shared_ptr<WizardDraft> draft;
    aria::Property<std::string>  finishedSummary{""};
    /// Finish command — wired to finish(); lets every platform drive
    /// the same command surface.
    aria::Command<> finishCmd{[this]{ finish(); }};

    explicit Step3Vm(wb::i18n::Context context, std::shared_ptr<WizardDraft> d);
    void finish();
};

class WizardVm {
public:
    explicit WizardVm(wb::i18n::Context context);
    [[nodiscard]] aria::i18n::LocaleContext& i18n() const { return *context_; }

    std::shared_ptr<WizardDraft>              draft;
    std::shared_ptr<aria::binding::Navigator> nav;

    std::shared_ptr<Step1Vm> step1;
    std::shared_ptr<Step2Vm> step2;
    std::shared_ptr<Step3Vm> step3;

    void toStep(int i);

private:
    wb::i18n::Context context_;
};

}  // namespace wb::wizard
