#include "App/QtAppShell.h"
#include "App/QtViewManifest.h"
#include "support/QtViewFactory.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace wb::qt {

std::string QtAppShell::resolve_i18n_dir() {
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        appDir + "/i18n",
        appDir + "/../Resources/i18n",           // macOS .app bundle
        appDir + "/../../../../Workbench/i18n",   // Development: build/mac/platform/qt -> source tree
        appDir + "/../../../Workbench/i18n",
    };
    for (const auto& c : candidates) {
        if (QFileInfo(c + "/common/strings.xml").exists())
            return QDir(c).absolutePath().toStdString();
    }
    return QDir(appDir + "/i18n").absolutePath().toStdString();
}

QtAppShell::QtAppShell(QObject* qt_ctx)
    : dispatcher_(std::make_shared<aria::adapters::qt6::QtDispatcher>(qt_ctx)),
      adapter_(std::make_shared<aria::adapters::qt6::QtAdapter>()),
      be_(adapter_, dispatcher_,
          aria::binding::BindingEngine::DispatchPolicy::SmartMarshal),
      core_(resolve_i18n_dir(), "zh-CN")
{
    register_all_views();  // Register each module's Qt View builder.
}

QtAppShell::~QtAppShell() = default;

QWidget* QtAppShell::build_page(int index) {
    const auto& mods = core_.modules();
    if (index < 0 || index >= static_cast<int>(mods.size())) return new QWidget;
    const auto& entry = mods[static_cast<std::size_t>(index)];
    return QtViewFactory::instance().build(entry.id, *entry.vm, be_);
}

}  // namespace wb::qt
