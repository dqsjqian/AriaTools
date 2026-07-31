#include "support/QtViewFactory.h"

namespace wb::qt {

QtViewFactory& QtViewFactory::instance() {
    static QtViewFactory f;
    return f;
}

}  // namespace wb::qt
