#pragma once
//
// IModule — Business module plugin contract (compile-time).
// Each module implements this and exports a make_<mod>_module() factory.
// The app layer lists them explicitly in ModulesManifest.
//
#include "aria/binding/view_model.hpp"
#include "module_api/ModuleContext.h"
#include "module_api/NavigatorHost.h"

#include <memory>
#include <string>

namespace wb::module_api {

class IModule {
public:
    virtual ~IModule() = default;

    /// Module id (= i18n subdirectory name, e.g. "notes").
    [[nodiscard]] virtual std::string id() const = 0;

    /// Navigation text key (in the common module, e.g. "nav_notes").
    [[nodiscard]] virtual std::string nav_key() const = 0;

    /// Navigation order (ascending).
    [[nodiscard]] virtual int order() const = 0;

    /// Create this module's ViewModel (injecting services / event bus).
    [[nodiscard]] virtual std::shared_ptr<aria::binding::ViewModel>
        create_view_model(ModuleContext& ctx) = 0;

    /// Cross-module navigation capabilities. A module overrides this to
    /// register its pages as navigation targets (e.g.
    /// `nav.Register<ICartPage>("cart", [&ctx]{ return create_view_model(ctx); })`).
    /// Called once by AppCore during load_modules; default is no targets.
    virtual void register_navigation(NavigatorHost& nav) { (void)nav; }
};

}  // namespace wb::module_api
