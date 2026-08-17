#pragma once
//
// ICartPage — cross-module navigation target: "the cart page".
//
// Lives in module_api so any module can navigate to the cart by including
// only this framework header. The cart module's VM implements it and
// self-registers with NavigatorHost; if the cart module is removed,
// NavigatorHost::Push<ICartPage> simply returns false.
//
// TWO INDEPENDENT navigation channels — the caller picks either, and the
// target receives exactly the type that was pushed:
//
//   1. Strongly-typed struct channel (optional):
//        Push<ICartPage>(CartArgs{...}) → on_navigate(const CartArgs&)
//      Caller assembles the struct; target consumes it directly. Use this
//      when both sides agree on a fixed, compile-time-checked shape.
//
//   2. Free-form json channel (universal, inherited from INavigationTarget):
//        Push<ICartPage>(json{...})     → on_navigate(const json&)
//      Caller and consumer handle the payload freely — the caller may send
//      ten fields, the consumer reads only the three it needs. There is NO
//      forced struct round-trip; json assembly/parsing is the business's own
//      job. Shared key constants (kParam*) exist only to prevent typos —
//      they are a superset both sides may use, never a required schema.
//

#include "module_api/INavigationTarget.h"

#include <nlohmann/json.hpp>

#include <string>

namespace wb::module_api {

/// Typed payload for the strongly-typed channel. Optional: a caller may
/// navigate with a raw json object instead, which the target parses freely.
struct CartArgs {
    std::string product;
    double      price = 0.0;
};

class ICartPage : public INavigationTarget {
public:
    ~ICartPage() override = default;

    /// Re-expose the inherited json on_navigate overload; declaring
    /// on_navigate(const CartArgs&) below would otherwise hide it
    /// (C++ name hiding across overload sets).
    using INavigationTarget::on_navigate;

    /// Shared json key constants (superset for both sides, typo-guard only).
    static constexpr const char* kParamProduct = "product";
    static constexpr const char* kParamPrice   = "price";

    /// Strongly-typed struct channel — OPTIONAL, independent from the json
    /// channel. Return true if the payload was consumed. Default false:
    /// this VM does not handle typed CartArgs. There is NO bridge to json —
    /// a typed push delivers CartArgs as-is, a json push delivers json
    /// as-is, and NavigatorHost::Push fails when the target's on_navigate
    /// returns false.
    virtual bool on_navigate(const CartArgs& /*args*/) {
        return false;
    }
};

}  // namespace wb::module_api
