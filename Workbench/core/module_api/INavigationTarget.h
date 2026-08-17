#pragma once
//
// INavigationTarget — the contract a ViewModel implements to become a
// cross-module navigation target.
//
// This interface lives in module_api (the framework kernel), so a caller
// that wants to navigate to it only ever includes THIS header — never a
// business module's ViewModel header. The business module's VM implements
// the interface and self-registers with NavigatorHost.
//
// Success signalling: every on_navigate overload returns bool. A VM returns
// true when it CONSUMED the payload (i.e. it implements this channel); the
// default false means "I do not handle this payload". NavigatorHost::Push
// treats a false return as a failed navigation — it does not push the page
// and reports failure to the caller, so the caller knows up front whether
// its payload will actually be delivered.
//
// No pure virtuals, no implicit conversion between channels: a VM overrides
// exactly the overload(s) it consumes.
//

#include <nlohmann/json.hpp>

namespace wb::module_api {

class INavigationTarget {
public:
    virtual ~INavigationTarget() = default;

    /// Free-form json channel. Return true if the payload was consumed.
    /// Default false: this VM does not handle json payloads.
    virtual bool on_navigate(const nlohmann::json& /*payload*/) {
        return false;
    }
};

}  // namespace wb::module_api
