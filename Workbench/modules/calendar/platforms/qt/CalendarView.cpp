#include "CalendarView.h"
#include "support/UiHelpers.h"
#include "viewmodels/CalendarVm.h"
#include "models/CalendarTypes.h"

#include "aria/binding/binding_engine.hpp"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <array>
#include <memory>
#include <string>

namespace wb::calendar::qtview {

namespace {

struct DayWidgets { QFrame* frame; QLabel* dayLabel; QLabel* events; };

// ─── Day cell painting ────────────────────────────────────────────────────
void paint_cell(CalendarVm& vm, DayWidgets& w, const DayCell& c) {
    w.dayLabel->setText(QString::fromStdString(c.label));
    w.events->setText(QString::fromStdString(vm.display_events(c)));

    QString bg = c.inCurrentMonth ? "#ffffff" : "#f5f5f5";
    QString fg = c.inCurrentMonth ? "#212121" : "#bdbdbd";
    QString border = "#e0e0e0";
    if (c.isToday) { bg = "#e8eaf6"; border = "#3f51b5"; }
    w.frame->setStyleSheet(
        QString("QFrame { background:%1; border:1px solid %2; border-radius:4px; }")
            .arg(bg, border));
    w.dayLabel->setStyleSheet(
        QString("QLabel { color:%1; font-weight:%2; border:none; }")
            .arg(fg, c.isToday ? "bold" : "normal"));
    w.events->setStyleSheet("QLabel { color:#455a64; font-size:10px; border:none; }");
}

// ─── Month nav sub-view ────────────────────────────────────────────────────
QWidget* build_month_nav(CalendarVm& vm, aria::binding::BindingEngine& be) {
    auto* nav_ = new QWidget;
    auto* row = new QHBoxLayout(nav_);
    auto* prevBtn = new QPushButton;
    auto* monthLbl = new QLabel;
    auto* nextBtn = new QPushButton;
    auto* todayBtn = new QPushButton;
    auto* refreshBtn = new QPushButton;
    monthLbl->setAlignment(Qt::AlignCenter);
    monthLbl->setStyleSheet("QLabel { font-size:16px; font-weight:bold; }");
    row->addWidget(prevBtn);
    row->addWidget(monthLbl, 1);
    row->addWidget(nextBtn);
    row->addWidget(todayBtn);
    row->addWidget(refreshBtn);

    be.bind_text_oneway(vm.prevLabel,    wb::ui::view_for(prevBtn));
    be.bind_text_oneway(vm.nextLabel,    wb::ui::view_for(nextBtn));
    be.bind_text_oneway(vm.todayLabel,   wb::ui::view_for(todayBtn));
    be.bind_text_oneway(vm.refreshLabel, wb::ui::view_for(refreshBtn));
    be.bind_text_oneway(vm.monthTitle,   wb::ui::view_for(monthLbl));
    be.bind_command(vm.prevMonth,  wb::ui::view_for(prevBtn));
    be.bind_command(vm.nextMonth,  wb::ui::view_for(nextBtn));
    be.bind_command(vm.today,      wb::ui::view_for(todayBtn));
    be.bind_command(vm.refresh,    wb::ui::view_for(refreshBtn));
    return nav_;
}

// ─── Day grid sub-view ─────────────────────────────────────────────────────
QWidget* build_day_grid(CalendarVm& vm, aria::binding::BindingEngine& be,
                        std::vector<aria::Subscription>& subs) {
    auto* grid_ = new QWidget;
    auto cells = std::make_shared<std::array<DayWidgets, 42>>();
    auto* layout = new QGridLayout(grid_);
    layout->setSpacing(3);

    // Weekday header row.
    std::array<aria::Property<std::string>*, 7> wds{
        &vm.wd1, &vm.wd2, &vm.wd3, &vm.wd4, &vm.wd5, &vm.wd6, &vm.wd7};
    for (int col = 0; col < 7; ++col) {
        auto* h = new QLabel;
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("QLabel { color:#616161; font-weight:bold; }");
        layout->addWidget(h, 0, col);
        be.bind_text_oneway(*wds[static_cast<std::size_t>(col)], wb::ui::view_for(h));
    }

    // 42 day cells.
    for (int i = 0; i < 42; ++i) {
        const int row = i / 7 + 1;
        const int col = i % 7;
        auto& dw = (*cells)[static_cast<std::size_t>(i)];
        dw.frame = new QFrame;
        dw.frame->setMinimumSize(64, 56);
        auto* cellLay = new QVBoxLayout(dw.frame);
        cellLay->setContentsMargins(4, 2, 4, 2);
        cellLay->setSpacing(1);
        dw.dayLabel = new QLabel;
        dw.events = new QLabel;
        dw.events->setWordWrap(true);
        cellLay->addWidget(dw.dayLabel);
        cellLay->addWidget(dw.events, 1);
        layout->addWidget(dw.frame, row, col);
    }

    auto repaint = [cells, &vm]() {
        for (std::size_t i = 0; i < 42 && i < vm.days.size(); ++i) {
            if (auto c = vm.days.at(i)) paint_cell(vm, (*cells)[i], *c);
        }
    };
    repaint();
    subs.push_back(vm.days.on_any_change([repaint]() { repaint(); }));
    return grid_;
}

// ─── Subscription bar sub-view ─────────────────────────────────────────────
QWidget* build_subscription_bar(CalendarVm& vm, aria::binding::BindingEngine& be,
                                std::vector<aria::Subscription>& subs) {
    auto* bar_ = new QWidget;
    auto* row = new QHBoxLayout(bar_);
    auto* urlEdit = new QLineEdit;
    auto* subBtn = new QPushButton;
    row->addWidget(urlEdit, 1);
    row->addWidget(subBtn);

    urlEdit->setPlaceholderText(QString::fromStdString(vm.urlPlaceholder.get()));
    subs.push_back(vm.urlPlaceholder.on_changed([urlEdit](const std::string& s) {
        urlEdit->setPlaceholderText(QString::fromStdString(s));
    }));
    be.bind_text_oneway(vm.subscribeLabel, wb::ui::view_for(subBtn));
    wb::ui::bind_editable_text(be, vm.subscribeUrl, urlEdit);
    be.bind_command(vm.addSubscription, wb::ui::view_for(subBtn));
    return bar_;
}

// ─── Subscription list sub-view ────────────────────────────────────────────
QListWidget* build_subscription_list(CalendarVm& vm, aria::binding::BindingEngine&,
                                     std::vector<aria::Subscription>& subs) {
    auto* list_ = new QListWidget;
    list_->setMaximumHeight(90);
    auto rebuildSubs = [list_, &vm]() {
        list_->clear();
        for (std::size_t i = 0; i < vm.subscriptions.size(); ++i) {
            if (auto s = vm.subscriptions.at(i)) {
                auto* item = new QListWidgetItem(
                    QString::fromStdString(vm.display_sub_name(*s)));
                item->setData(Qt::UserRole + 1, QString::fromStdString(s->id));
                item->setToolTip(QString::fromStdString(s->url));
                list_->addItem(item);
            }
        }
    };
    rebuildSubs();
    subs.push_back(vm.subscriptions.on_any_change([rebuildSubs]() { rebuildSubs(); }));
    QObject::connect(list_, &QListWidget::itemDoubleClicked,
                     [&vm](QListWidgetItem* item) {
                         if (!item) return;
                         vm.removeSubscription.execute(
                             item->data(Qt::UserRole + 1).toString().toStdString());
                     });
    return list_;
}

}  // namespace

// ─── Top-level view ────────────────────────────────────────────────────────
CalendarView::CalendarView(CalendarVm& vm, aria::binding::BindingEngine& be)
    : root_(new QWidget) {
    auto& subs = wb::ui::subs_attached_to(root_);
    auto* lay = new QVBoxLayout(root_);

    // Title + hint.
    auto* title = wb::ui::make_title("");
    auto* hint = wb::ui::make_info("");
    lay->addWidget(title);
    lay->addWidget(hint);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint,  wb::ui::view_for(hint));

    // Sub-views.
    lay->addWidget(build_month_nav(vm, be));
    lay->addWidget(build_day_grid(vm, be, subs), 1);
    lay->addWidget(build_subscription_bar(vm, be, subs));
    lay->addWidget(build_subscription_list(vm, be, subs));

    // Status.
    auto* statusLbl = new QLabel;
    lay->addWidget(statusLbl);
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));
}

}  // namespace wb::calendar::qtview
