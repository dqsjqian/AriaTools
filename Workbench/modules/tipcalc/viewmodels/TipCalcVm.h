#pragma once
//
// TipCalcVm — Tab 1: tip calculator
//
// Scenario
//   bill / tip % / number of people → live computation of tip /
//   total / per-person amount.
//   The "Round up" button rounds the bill to the nearest integer and
//   snaps the tip to a 5% boundary — two Property writes wrapped in
//   reactive::batch so downstream flushes once.
//
// Framework features
//   - binding::ViewModel    activate/deactivate lifecycle, bag_
//   - Property / Computed / Command / reactive::batch / Effect
//
// Lifecycle
//   Command<>::CanExecute is automatically tracked by an internal Effect;
//   no manual activate/deactivate wiring is needed.
//

#include "aria/aria.hpp"
#include "aria/command.hpp"
#include "module_api/BaseVm.h"

namespace wb::tipcalc {

class TipCalcVm : public wb::core::BaseVm {
public:
    TipCalcVm();

    // i18n-driven UI text (View renders these; no literals in Views).
    aria::Property<std::string> title;        ///< "小费计算器"
    aria::Property<std::string> desc;         ///< 功能说明
    aria::Property<std::string> billLabel;    ///< "账单"
    aria::Property<std::string> tipLabel;     ///< "小费 %"
    aria::Property<std::string> peopleLabel;  ///< "人数"
    aria::Property<std::string> tipAmountText;///< "小费金额"
    aria::Property<std::string> totalText;    ///< "总计"
    aria::Property<std::string> perPersonText;///< "每人付"
    aria::Property<std::string> roundUpText;  ///< 按钮文本

    aria::Property<double> bill;
    aria::Property<int>    tipPercent;
    aria::Property<int>    people;

    aria::Computed<double> tipAmount;
    aria::Computed<double> total;
    aria::Computed<double> perPerson;

    aria::Command<> roundUp;

private:
    bool canRoundUp_() const;
};

}  // namespace wb::tipcalc
