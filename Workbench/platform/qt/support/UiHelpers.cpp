#include "support/UiHelpers.h"

#include <memory>

namespace wb::ui {

QLabel* make_title(const QString& text, QWidget* parent) {
    auto* l = new QLabel(text, parent);
    // Don't hard-code colors: follow the system theme (light -> black text, dark -> white text), to avoid invisible text on a black background.
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

/// Subscription bag as a child QObject of the owner: owner destroyed -> bag destroyed -> subscriptions released.
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

    // 1) Initial sync VM->View once (no input at this point, doesn't affect cursor).
    adapter.set_text(v, prop.get());

    // 2) View->VM only: user input writes back to the Property. Does not subscribe to prop.on_changed,
    //    so VM changes don't push back setPlainText/setText -> cursor is not reset.
    auto sub = adapter.on_text_changed(v, [&prop](std::string_view sv) {
        prop.set(std::string(sv));
    });
    // Subscription lifetime is attached to the widget; released when the widget is destroyed.
    subs_attached_to(widget).push_back(std::move(sub));
}

}  // namespace wb::ui
