#include "viewmodels/ChatVm.h"

#include "events/CrossModuleEvents.h"

namespace wb::chat {

// ── Publisher ────────────────────────────────────────────────────────────
ChatPublisherVm::ChatPublisherVm(aria::runtime::EventBus& bus)
    : bus_(bus),
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

void ChatPublisherVm::on_activate() {
}

void ChatPublisherVm::on_deactivate() { bag().clear(); }

// ── Subscriber ───────────────────────────────────────────────────────────
ChatSubscriberVm::ChatSubscriberVm(aria::runtime::EventBus& bus) : bus_(bus) {}

void ChatSubscriberVm::on_activate() {
    // Subscription is attached to bag_; on_deactivate's bag().clear()
    // disconnects automatically.
    bag() += bus_.subscribe<ChatMessage>([this](const ChatMessage& m) {
        messages.push_back(std::make_shared<ChatMessage>(m));
    });

    // ── Cross-module: react to cart events ────────────────────────────
    // When a user adds an item to the cart (in the cart module), this
    // subscriber auto-posts a system message — demonstrating that modules
    // can communicate via the EventBus without direct coupling.
    bag() += bus_.subscribe<wb::shared::events::ItemAddedToCart>(
        [this](const wb::shared::events::ItemAddedToCart& ev) {
            messages.push_back(std::make_shared<ChatMessage>(
                ChatMessage{"[system]",
                    ev.productName + " added to cart (¥"
                    + std::to_string(ev.price) + ")"}));
        });
    bag() += bus_.subscribe<wb::shared::events::OrderPlaced>(
        [this](const wb::shared::events::OrderPlaced& ev) {
            messages.push_back(std::make_shared<ChatMessage>(
                ChatMessage{"[system]",
                    "Order " + ev.orderId + " placed ("
                    + std::to_string(ev.itemCount) + " items)"}));
        });
    // Model → EventBus → chat: a cart item's qty changed.
    bag() += bus_.subscribe<wb::shared::events::ItemQtyChanged>(
        [this](const wb::shared::events::ItemQtyChanged& ev) {
            messages.push_back(std::make_shared<ChatMessage>(
                ChatMessage{"[system]",
                    ev.productName + " qty changed"}));
        });
}

void ChatSubscriberVm::on_deactivate() { bag().clear(); }

// ── Composite VM: activate cascades to children ──────────────────────────────
ChatVm::ChatVm(aria::runtime::EventBus& bus)
    : publisher(std::make_shared<ChatPublisherVm>(bus)),
      subscriber(std::make_shared<ChatSubscriberVm>(bus))
{
    text(title, "title");
    text(desc, "desc");
    add_child(publisher);
    add_child(subscriber);
}

}  // namespace wb::chat
