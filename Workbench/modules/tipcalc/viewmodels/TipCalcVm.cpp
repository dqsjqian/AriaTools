#include "viewmodels/TipCalcVm.h"

#include <cmath>

namespace wb::tipcalc {

TipCalcVm::TipCalcVm()
    : bill(238.0),
      tipPercent(15),
      people(2),
      tipAmount([this] { return bill.get() * tipPercent.get() / 100.0; }),
      total    ([this] { return bill.get() + tipAmount.get(); }),
      perPerson([this] {
          const int n = people.get();
          return n > 0 ? total.get() / n : 0.0;
      }),
      roundUp(
          [this] {
              aria::batch([this] {
                  bill       = std::ceil(bill.get());
                  tipPercent = ((tipPercent.get() + 4) / 5) * 5;
              });
          },
          [this] { return canRoundUp_(); })
{
    // i18n text (auto-refresh on language change via BaseVm).
    text(title,         "title");
    text(desc,          "desc");
    text(billLabel,     "bill_label");
    text(tipLabel,      "tip_label");
    text(peopleLabel,   "people_label");
    text(tipAmountText, "tip_amount");
    text(totalText,     "total");
    text(perPersonText, "per_person");
    text(roundUpText,   "round_up");
}

bool TipCalcVm::canRoundUp_() const {
    const double b = bill.get();
    return (b != std::floor(b)) || (tipPercent.get() % 5 != 0);
}

}  // namespace wb::tipcalc
