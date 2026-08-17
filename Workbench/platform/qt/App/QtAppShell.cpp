#include "App/QtAppShell.h"
#include "App/QtViewManifest.h"
#include "support/QtViewFactory.h"
#include "support/QtExecutors.h"

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
      ui_exec_(*dispatcher_),
      delay_(*dispatcher_),
      adapter_(std::make_shared<aria::adapters::qt6::QtAdapter>()),
      be_(adapter_, dispatcher_,
          aria::binding::BindingEngine::DispatchPolicy::SmartMarshal),
      core_(resolve_i18n_dir(), "zh-CN")
{
    // Inject QtDispatcher (wrapped as IExecutor + IDelayedScheduler) into
    // ServiceHub BEFORE load_modules() so AsyncCommand VMs (e.g. login) get
    // a real UI-thread co_await target. The InlineExecutor fallback would
    // trip the graph thread-affinity invariant when worker runs on a
    // different thread.
    core_.set_ui_executor(&ui_exec_);
    core_.set_timer(&delay_);
    core_.load_modules();
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
