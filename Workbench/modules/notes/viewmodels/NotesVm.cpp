#include "viewmodels/NotesVm.h"

#include "infra/i18n/I18n.h"
#include "events/CrossModuleEvents.h"

#include "aria/runtime/event_bus.hpp"

namespace wb::notes {

NotesVm::NotesVm(std::shared_ptr<NotesModel> model)
    : notes(model->notes),
      selectedId(model->selectedId),
      editTitle(""),
      editBody(""),
      hasSelection(model->hasSelection),
      dirty(model->dirty),
      status(""),
      addNote([this] {
          (void)model_->create_note();
          load_selection_into_editor_();
          refresh_status_();
      }),
      selectNote([this](const std::string& id) {
          model_->select(id);
          load_selection_into_editor_();
      }),
      saveNote([this] {
          model_->set_title(editTitle.get());
          model_->set_body(editBody.get());
          (void)model_->save_current();
      }),
      deleteSelected([this] {
          (void)model_->delete_current();
          load_selection_into_editor_();
          refresh_status_();
      }),
      model_(std::move(model))
{
    // Editor -> Model draft (not persisted until save, only marks dirty).
    track(editTitle.on_changed([this](const std::string& v) { model_->set_title(v); }));
    track(editBody.on_changed([this](const std::string& v) { model_->set_body(v); }));

    // Static text: written near the use site, auto-refreshed on language switch.
    text(title,            "title");
    text(hint,             "hint");
    text(addLabel,         "add");
    text(saveLabel,        "save");
    text(deleteLabel,      "delete");
    text(titlePlaceholder, "title_placeholder");
    text(bodyPlaceholder,  "body_placeholder");

    // Dynamic status: refreshes both on list-size change and on language change.
    localize([this] { refresh_status_(); });
    track(notes.on_any_change([this]() { refresh_status_(); }));

    // ── Cross-module: subscribe to cart events ────────────────────────
    // When the user operates the cart module (add item / change qty /
    // checkout), notes automatically creates an operation-log entry.
    // This demonstrates Model → EventBus → multi-module VM notification:
    // the cart module has NO direct reference to the notes module.
    auto& bus = aria::runtime::EventBus::global();
    cart_add_sub_ = bus.subscribe<wb::shared::events::ItemAddedToCart>(
        [this](const wb::shared::events::ItemAddedToCart& ev) {
            (void)model_->create_note();
            model_->set_title("[cart] " + ev.productName);
            model_->set_body("Added " + ev.productName
                           + " (price: " + std::to_string(ev.price)
                           + ", qty: " + std::to_string(ev.qty) + ")");
            (void)model_->save_current();
        });
    cart_qty_sub_ = bus.subscribe<wb::shared::events::ItemQtyChanged>(
        [this](const wb::shared::events::ItemQtyChanged& ev) {
            (void)model_->create_note();
            model_->set_title("[cart] qty changed: " + ev.productName);
            model_->set_body("Quantity for " + ev.productName + " changed.");
            (void)model_->save_current();
        });
    cart_order_sub_ = bus.subscribe<wb::shared::events::OrderPlaced>(
        [this](const wb::shared::events::OrderPlaced& ev) {
            (void)model_->create_note();
            model_->set_title("[cart] order placed: " + ev.orderId);
            model_->set_body("Order " + ev.orderId
                           + " (" + std::to_string(ev.itemCount)
                           + " items, total: " + std::to_string(ev.total) + ")");
            (void)model_->save_current();
        });

    load_selection_into_editor_();
}

void NotesVm::on_activate() {
    (void)model_->reload();
    load_selection_into_editor_();
    refresh_status_();
}

void NotesVm::on_deactivate() { bag().clear(); }

void NotesVm::load_selection_into_editor_() {
    editTitle.set(model_->draftTitle.get());
    editBody.set(model_->draftBody.get());
}

void NotesVm::refresh_status_() {
    const auto n = notes.size();
    if (n == 0) {
        status.set(wb::i18n::str("empty"));
    } else {
        status.set(wb::i18n::str("count_prefix") + std::to_string(n) + wb::i18n::str("count_suffix"));
    }
}


std::string NotesVm::display_title(const Note& n) const {
    return n.title.empty() ? wb::i18n::str_in("common", "untitled")
                           : n.title;
}

}  // namespace wb::notes
