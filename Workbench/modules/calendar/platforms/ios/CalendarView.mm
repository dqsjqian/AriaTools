#include "CalendarView.h"
#include "support/UIViewFactory.h"
#include "support/IosUi.h"
#include "viewmodels/CalendarVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::calendar::iosview {

// ─── MonthNavView ──────────────────────────────────────────────────────────
MonthNavView::MonthNavView(CalendarVm& vm, aria::binding::BindingEngine& be)
    : nav_([[UIStackView alloc] init]) {
    ((UIStackView*)nav_).axis = UILayoutConstraintAxisHorizontal;
    ((UIStackView*)nav_).spacing = 8;
    ((UIStackView*)nav_).distribution = UIStackViewDistributionFill;

    UILabel* monthLbl = wb::ios::ui::make_label(@"");
    UIButton* prevBtn = wb::ios::ui::make_button(@"");
    UIButton* nextBtn = wb::ios::ui::make_button(@"");
    UIButton* todayBtn = wb::ios::ui::make_button(@"");
    UIButton* refreshBtn = wb::ios::ui::make_button(@"");

    [(UIStackView*)nav_ addArrangedSubview:prevBtn];
    [(UIStackView*)nav_ addArrangedSubview:monthLbl];
    [(UIStackView*)nav_ addArrangedSubview:nextBtn];
    [(UIStackView*)nav_ addArrangedSubview:todayBtn];
    [(UIStackView*)nav_ addArrangedSubview:refreshBtn];

    be.bind_text_oneway(vm.monthTitle,   wb::ios::ui::view_for(monthLbl));
    be.bind_text_oneway(vm.prevLabel,    wb::ios::ui::view_for(prevBtn));
    be.bind_text_oneway(vm.nextLabel,    wb::ios::ui::view_for(nextBtn));
    be.bind_text_oneway(vm.todayLabel,   wb::ios::ui::view_for(todayBtn));
    be.bind_text_oneway(vm.refreshLabel, wb::ios::ui::view_for(refreshBtn));
    be.bind_command(vm.prevMonth,  wb::ios::ui::view_for(prevBtn));
    be.bind_command(vm.nextMonth,  wb::ios::ui::view_for(nextBtn));
    be.bind_command(vm.today,      wb::ios::ui::view_for(todayBtn));
    be.bind_command(vm.refresh,    wb::ios::ui::view_for(refreshBtn));
}

// ─── SubscriptionBarView ──────────────────────────────────────────────────
SubscriptionBarView::SubscriptionBarView(CalendarVm& vm, aria::binding::BindingEngine& be)
    : bar_([[UIStackView alloc] init]) {
    ((UIStackView*)bar_).axis = UILayoutConstraintAxisHorizontal;
    ((UIStackView*)bar_).spacing = 8;
    ((UIStackView*)bar_).distribution = UIStackViewDistributionFill;

    UITextField* urlField = wb::ios::ui::make_field(@"");
    UIButton* subBtn = wb::ios::ui::make_button(@"");
    [(UIStackView*)bar_ addArrangedSubview:urlField];
    [(UIStackView*)bar_ addArrangedSubview:subBtn];

    be.bind_text_oneway(vm.subscribeLabel, wb::ios::ui::view_for(subBtn));
    be.bind_text(vm.subscribeUrl, wb::ios::ui::view_for(urlField));
    be.bind_command(vm.addSubscription, wb::ios::ui::view_for(subBtn));
}

// ─── Top-level CalendarView ───────────────────────────────────────────────
CalendarView::CalendarView(CalendarVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel* title = wb::ios::ui::make_title(@"");
    UILabel* hint  = wb::ios::ui::make_label(@"");
    UILabel* status = wb::ios::ui::make_label(@"");

    MonthNavView nav(vm, be);
    SubscriptionBarView subBar(vm, be);

    NSMutableArray<UIView*>* children = [NSMutableArray arrayWithCapacity:6];
    [children addObject:title];
    [children addObject:hint];
    [children addObject:nav.view()];
    [children addObject:subBar.view()];
    [children addObject:status];

    vc_ = wb::ios::ui::make_stack_vc(children);

    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.hint,  wb::ios::ui::view_for(hint));
    be.bind_text_oneway(vm.status, wb::ios::ui::view_for(status));
}

}  // namespace wb::calendar::iosview

namespace wb::calendar {
void register_calendar_view() {
    wb::ios::UIViewFactory::instance().register_builder(
        "calendar", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            auto* view = new iosview::CalendarView(static_cast<CalendarVm&>(vm), be);
            return view->viewController();
        });
}
}  // namespace wb::calendar
