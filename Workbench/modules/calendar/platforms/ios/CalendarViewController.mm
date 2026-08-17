#import "CalendarViewController.h"
#import "CalendarView.h"

#include "viewmodels/CalendarVm.h"
#include "infra/i18n/I18n.h"

#include "aria/binding/binding_engine.hpp"
#include "aria/adapters/uikit/UIKitAdapter.hpp"

#include "support/IosUi.h"

@implementation CalendarViewController {
    wb::calendar::CalendarVm* _vm;   // raw ref; lifetime managed by AppCore
    aria::binding::BindingEngine* _be;
    CalendarView *_calendarView;
}

- (instancetype)initWithViewModelRef:(wb::calendar::CalendarVm&)vm
                                engine:(aria::binding::BindingEngine&)be {
    if ((self = [super initWithNibName:nil bundle:nil])) {
        _vm = &vm;
        _be = &be;
    }
    return self;
}

- (void)loadView {
    // Controller creates the View (MVC: Controller owns View).
    _calendarView = [[CalendarView alloc] initWithFrame:[UIScreen mainScreen].bounds];
    self.view = _calendarView;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    auto& vm = *_vm;
    auto& be = *_be;

    // ── Bindings: VM ↔ View outlets ────────────────────────────────────
    be.bind_text_oneway(vm.title,      wb::ios::ui::view_for(_calendarView.titleLabel));
    be.bind_text_oneway(vm.hint,       wb::ios::ui::view_for(_calendarView.hintLabel));
    be.bind_text_oneway(vm.monthTitle, wb::ios::ui::view_for(_calendarView.monthTitleLabel));
    be.bind_text_oneway(vm.status,     wb::ios::ui::view_for(_calendarView.statusLabel));

    be.bind_text_oneway(vm.prevLabel,    wb::ios::ui::view_for(_calendarView.prevButton));
    be.bind_text_oneway(vm.nextLabel,    wb::ios::ui::view_for(_calendarView.nextButton));
    be.bind_text_oneway(vm.todayLabel,   wb::ios::ui::view_for(_calendarView.todayButton));
    be.bind_text_oneway(vm.refreshLabel, wb::ios::ui::view_for(_calendarView.refreshButton));
    be.bind_text_oneway(vm.subscribeLabel, wb::ios::ui::view_for(_calendarView.subscribeButton));

    be.bind_text(vm.subscribeUrl, wb::ios::ui::view_for(_calendarView.urlField));

    be.bind_command(vm.prevMonth,        wb::ios::ui::view_for(_calendarView.prevButton));
    be.bind_command(vm.nextMonth,        wb::ios::ui::view_for(_calendarView.nextButton));
    be.bind_command(vm.today,            wb::ios::ui::view_for(_calendarView.todayButton));
    be.bind_command(vm.refresh,          wb::ios::ui::view_for(_calendarView.refreshButton));
    be.bind_command(vm.addSubscription,  wb::ios::ui::view_for(_calendarView.subscribeButton));

    // URL placeholder (i18n).
    _calendarView.urlField.placeholder =
        [NSString stringWithUTF8String:vm.urlPlaceholder.get().c_str()];
}

@end
