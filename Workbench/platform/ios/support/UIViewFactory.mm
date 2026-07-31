#include "support/UIViewFactory.h"

namespace wb::ios {

UIViewFactory& UIViewFactory::instance() {
    static UIViewFactory f;
    return f;
}

}  // namespace wb::ios
