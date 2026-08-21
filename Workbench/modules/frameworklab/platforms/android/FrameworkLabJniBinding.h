#pragma once

#include "jni/JniBind.h"
#include "viewmodels/FrameworkLabVm.h"

namespace wb::frameworklab {

void register_frameworklab_binding(wb::jni::BindingTable& table);

}  // namespace wb::frameworklab
