#pragma once
//
// ChatVm — Tab 7: chat room
//
// Three VMs:
//   ChatPublisherVm    publishes messages onto the EventBus
//   ChatSubscriberVm   subscribes to messages and stores them in an
//                      ObservableList
//   ChatVm             composite parent that owns the two above as
//                      children; activate / deactivate cascades
//                      automatically.
//

#include "aria/aria.hpp"
#include "module_api/BaseVm.h"
#include "aria/command.hpp"
#include "aria/observable_list.hpp"
#include "aria/binding/view_model.hpp"
#include "aria/runtime/event_bus.hpp"

#include "models/ChatMessage.h"

#include <memory>
#include <string>

namespace wb::chat {

class ChatPublisherVm : public wb::core::BaseVm {
    aria::runtime::EventBus& bus_;
public:
    aria::Property<std::string> user{"alice"};
    aria::Property<std::string> draft{""};
    aria::Command<>             send;

    explicit ChatPublisherVm(aria::runtime::EventBus& bus);

    void on_activate() override;
    void on_deactivate() override;
};

class ChatSubscriberVm : public wb::core::BaseVm {
    aria::runtime::EventBus& bus_;
public:
    aria::ObservableList<ChatMessage> messages;

    explicit ChatSubscriberVm(aria::runtime::EventBus& bus);

    void on_activate() override;
    void on_deactivate() override;
};

class ChatVm : public wb::core::BaseVm {
public:
    // UI text (i18n, auto-refresh on language change).
    aria::Property<std::string> title;
    aria::Property<std::string> desc;
    std::shared_ptr<ChatPublisherVm>  publisher;
    std::shared_ptr<ChatSubscriberVm> subscriber;

    explicit ChatVm(aria::runtime::EventBus& bus);
};

}  // namespace wb::chat
