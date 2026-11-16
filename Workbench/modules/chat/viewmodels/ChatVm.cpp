#include "viewmodels/ChatVm.h"

#include "events/CrossModuleEvents.h"

namespace wb::chat {

// ── Publisher ────────────────────────────────────────────────────────────
ChatPublisherVm::ChatPublisherVm(wb::i18n::Context context, aria::runtime::EventBus& bus)
    : BaseVm(std::move(context)), bus_(bus),
      send(
          [this] {
              auto u = user.get();
              auto t = draft.get();
              if (!is_active().get() || u.empty() || t.empty()) return;
              bus_.publish(ChatMessage{u, t});
              draft.set(std::string{});
          },
          [this] {
              return is_active().get() && !user.get().empty() && !draft.get().empty();
          })
{}

// ── Subscriber ───────────────────────────────────────────────────────────
ChatSubscriberVm::ChatSubscriberVm(wb::i18n::Context context, aria::runtime::EventBus& bus) : BaseVm(std::move(context)), bus_(bus) {
    bag() += bus_.subscribe<ChatMessage>([this](const ChatMessage& m) {
        if (!is_active().get()) return;
        messages.push_back(std::make_shared<ChatMessage>(m));
    });

    // ── Cross-module: react to cart events ────────────────────────────
    // When a user adds an item to the cart (in the cart module), this
    // subscriber auto-posts a system message — demonstrating that modules
    // can communicate via the EventBus without direct coupling.
    bag() += bus_.subscribe<wb::shared::events::ItemAddedToCart>(
        [this](const wb::shared::events::ItemAddedToCart& ev) {
            if (!is_active().get()) return;
            messages.push_back(std::make_shared<ChatMessage>(
                ChatMessage{"[system]",
                    ev.productName + " added to cart (¥"
                    + std::to_string(ev.price) + ")"}));
        });
    bag() += bus_.subscribe<wb::shared::events::OrderPlaced>(
        [this](const wb::shared::events::OrderPlaced& ev) {
            if (!is_active().get()) return;
            messages.push_back(std::make_shared<ChatMessage>(
                ChatMessage{"[system]",
                    "Order " + ev.orderId + " placed ("
                    + std::to_string(ev.itemCount) + " items)"}));
        });
    // Model → EventBus → chat: a cart item's qty changed.
    bag() += bus_.subscribe<wb::shared::events::ItemQtyChanged>(
        [this](const wb::shared::events::ItemQtyChanged& ev) {
            if (!is_active().get()) return;
            messages.push_back(std::make_shared<ChatMessage>(
                ChatMessage{"[system]",
                    ev.productName + " qty changed"}));
        });
}

// ── Composite VM: activate cascades to children ──────────────────────────────
ChatVm::ChatVm(wb::i18n::Context context, aria::runtime::EventBus& bus)
    : BaseVm(std::move(context)), publisher(std::make_shared<ChatPublisherVm>(i18n_context(), bus)),
      subscriber(std::make_shared<ChatSubscriberVm>(i18n_context(), bus))
{
    text(title, "title");
    text(desc, "desc");
    add_child(publisher);
    add_child(subscriber);
}

}  // namespace wb::chat
