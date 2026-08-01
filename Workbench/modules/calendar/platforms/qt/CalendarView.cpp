#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/CalendarVm.h"

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
#include <string>

namespace wb::calendar::qtview {

// One day cell: the day number on top, with event titles below (limited number shown).
struct DayWidgets {
    QFrame* frame = nullptr;
    QLabel* dayLabel = nullptr;
    QLabel* events = nullptr;
};

static QString join_events(const DayCell& c) {
    QString s;
    int shown = 0;
    for (const auto& t : c.eventTitles) {
        if (shown >= 3) { s += QStringLiteral("…"); break; }
        if (!s.isEmpty()) s += QStringLiteral("\n");
        s += QString::fromStdString(t);
        ++shown;
    }
    return s;
}

static void paint_cell(DayWidgets& w, const DayCell& c) {
    w.dayLabel->setText(QString::fromStdString(c.label));
    w.events->setText(join_events(c));

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

static QWidget* build(wb::calendar::CalendarVm& vm, aria::binding::BindingEngine& be) {
    auto* w = new QWidget;
    auto& subs = wb::ui::subs_attached_to(w);
    auto* root = new QVBoxLayout(w);

    // Top: title + hint
    auto* title = wb::ui::make_title("");
    auto* hint = wb::ui::make_info("");
    root->addWidget(title);
    root->addWidget(hint);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    be.bind_text_oneway(vm.hint, wb::ui::view_for(hint));

    // Nav row: prev month | month title | next month | today | refresh
    auto* navRow = new QHBoxLayout;
    auto* prevBtn = new QPushButton;
    auto* monthLbl = new QLabel;
    auto* nextBtn = new QPushButton;
    auto* todayBtn = new QPushButton;
    auto* refreshBtn = new QPushButton;
    monthLbl->setAlignment(Qt::AlignCenter);
    monthLbl->setStyleSheet("QLabel { font-size:16px; font-weight:bold; }");
    navRow->addWidget(prevBtn);
    navRow->addWidget(monthLbl, 1);
    navRow->addWidget(nextBtn);
    navRow->addWidget(todayBtn);
    navRow->addWidget(refreshBtn);
    root->addLayout(navRow);
    be.bind_text_oneway(vm.prevLabel, wb::ui::view_for(prevBtn));
    be.bind_text_oneway(vm.nextLabel, wb::ui::view_for(nextBtn));
    be.bind_text_oneway(vm.todayLabel, wb::ui::view_for(todayBtn));
    be.bind_text_oneway(vm.refreshLabel, wb::ui::view_for(refreshBtn));
    be.bind_text_oneway(vm.monthTitle, wb::ui::view_for(monthLbl));
    be.bind_command(vm.prevMonth, wb::ui::view_for(prevBtn));
    be.bind_command(vm.nextMonth, wb::ui::view_for(nextBtn));
    be.bind_command(vm.today, wb::ui::view_for(todayBtn));
    be.bind_command(vm.refresh, wb::ui::view_for(refreshBtn));

    // Month grid: row 0 is the weekday header; rows 1..6 are 6 weeks x 7 days.
    auto* grid = new QGridLayout;
    grid->setSpacing(3);
    root->addLayout(grid, 1);

    std::array<aria::Property<std::string>*, 7> wds{
        &vm.wd1, &vm.wd2, &vm.wd3, &vm.wd4, &vm.wd5, &vm.wd6, &vm.wd7};
    for (int col = 0; col < 7; ++col) {
        auto* h = new QLabel;
        h->setAlignment(Qt::AlignCenter);
        h->setStyleSheet("QLabel { color:#616161; font-weight:bold; }");
        grid->addWidget(h, 0, col);
        be.bind_text_oneway(*wds[static_cast<std::size_t>(col)], wb::ui::view_for(h));
    }

    // 42 cell widgets (fixed); on rebuild only contents are updated, widgets are not recreated.
    auto cells = std::make_shared<std::array<DayWidgets, 42>>();
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
        grid->addWidget(dw.frame, row, col);
    }

    auto repaint = [cells, &vm]() {
        for (std::size_t i = 0; i < 42 && i < vm.days.size(); ++i) {
            if (auto c = vm.days.at(i)) paint_cell((*cells)[i], *c);
        }
    };
    repaint();
    subs.push_back(vm.days.on_any_change([repaint]() { repaint(); }));

    // Subscription row: input + subscribe button
    auto* subRow = new QHBoxLayout;
    auto* urlEdit = new QLineEdit;
    auto* subBtn = new QPushButton;
    subRow->addWidget(urlEdit, 1);
    subRow->addWidget(subBtn);
    root->addLayout(subRow);
    urlEdit->setPlaceholderText(QString::fromStdString(vm.urlPlaceholder.get()));
    subs.push_back(vm.urlPlaceholder.on_changed([urlEdit](const std::string& s) {
        urlEdit->setPlaceholderText(QString::fromStdString(s));
    }));
    be.bind_text_oneway(vm.subscribeLabel, wb::ui::view_for(subBtn));
    wb::ui::bind_editable_text(be, vm.subscribeUrl, urlEdit);
    be.bind_command(vm.addSubscription, wb::ui::view_for(subBtn));

    // Subscription list (double-click to remove)
    auto* subList = new QListWidget;
    subList->setMaximumHeight(90);
    root->addWidget(subList);
    auto rebuildSubs = [subList, &vm]() {
        subList->clear();
        for (std::size_t i = 0; i < vm.subscriptions.size(); ++i) {
            if (auto s = vm.subscriptions.at(i)) {
                auto* item = new QListWidgetItem(
                    QString::fromStdString(s->name.empty() ? s->url : s->name));
                item->setData(Qt::UserRole + 1, QString::fromStdString(s->id));
                item->setToolTip(QString::fromStdString(s->url));
                subList->addItem(item);
            }
        }
    };
    rebuildSubs();
    subs.push_back(vm.subscriptions.on_any_change([rebuildSubs]() { rebuildSubs(); }));
    QObject::connect(subList, &QListWidget::itemDoubleClicked,
                     [&vm](QListWidgetItem* item) {
                         if (!item) return;
                         vm.removeSubscription.execute(
                             item->data(Qt::UserRole + 1).toString().toStdString());
                     });

    // Status
    auto* statusLbl = new QLabel;
    root->addWidget(statusLbl);
    be.bind_text_oneway(vm.status, wb::ui::view_for(statusLbl));

    return w;
}

}  // namespace wb::calendar::qtview

namespace wb::calendar {

void register_calendar_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "calendar",
        [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<CalendarVm&>(vm), be);
        });
}

}  // namespace wb::calendar
