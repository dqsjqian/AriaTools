#include "support/UiHelpers.h"

#include <memory>

namespace wb::ui {

QLabel* make_title(const QString& text, QWidget* parent) {
    auto* l = new QLabel(text, parent);
    // 不写死颜色：跟随系统主题（浅色→黑字，深色→白字），避免黑底看不清。
    l->setStyleSheet("QLabel { font-size:20px; font-weight:bold; }");
    return l;
}

QLabel* make_info(const QString& text, QWidget* parent) {
    auto* l = new QLabel(text, parent);
    l->setWordWrap(true);
    l->setStyleSheet(
        "QLabel { color:#37474f; background:#eceff1; border:1px solid #cfd8dc;"
        " border-radius:6px; padding:8px; font-size:12px; }");
    return l;
}

namespace {

std::vector<std::shared_ptr<aria::adapters::qt6::QtView>>& view_keepalive() {
    static std::vector<std::shared_ptr<aria::adapters::qt6::QtView>> v;
    return v;
}

/// 订阅袋作为 owner 的子 QObject：owner 析构 → 袋析构 → 订阅解除。
class QtSubBag : public QObject {
public:
    explicit QtSubBag(QObject* parent) : QObject(parent) {}
    std::vector<aria::Subscription> subs;
};

}  // namespace

aria::adapters::qt6::QtView& view_for(QObject* w) {
    auto v = std::make_shared<aria::adapters::qt6::QtView>(w);
    auto& ref = *v;
    view_keepalive().push_back(std::move(v));
    return ref;
}

std::vector<aria::Subscription>& subs_attached_to(QObject* owner) {
    return (new QtSubBag(owner))->subs;
}

void bind_editable_text(aria::binding::BindingEngine& be,
                        aria::Property<std::string>& prop,
                        QObject* widget) {
    auto& adapter = be.adapter();
    auto& v = view_for(widget);

    // 1) 初始同步 VM→View 一次（此时无输入，不影响光标）。
    adapter.set_text(v, prop.get());

    // 2) 仅 View→VM：用户输入写回 Property。不订阅 prop.on_changed，
    //    因此 VM 变化不会回推 setPlainText/setText → 光标不被重置。
    auto sub = adapter.on_text_changed(v, [&prop](std::string_view sv) {
        prop.set(std::string(sv));
    });
    // 订阅生命周期挂到 widget，widget 析构即解除。
    subs_attached_to(widget).push_back(std::move(sub));
}

}  // namespace wb::ui
