#include "ToolsView.h"
#include "support/IosUi.h"
#include "viewmodels/ToolsVm.h"

#include "aria/binding/binding_engine.hpp"

namespace wb::tools::iosview {

ToolsView::ToolsView(ToolsVm& vm, aria::binding::BindingEngine& be)
    : vc_(nil) {
    UILabel*     title  = wb::ios::ui::make_title(@"");
    UILabel*     b64lbl = wb::ios::ui::make_label(@"");
    UITextField* b64in  = wb::ios::ui::make_field(@"");
    UITextField* b64out = wb::ios::ui::make_field(@""); b64out.enabled = NO;
    UIButton*    enc    = wb::ios::ui::make_button(@"");
    UIButton*    dec    = wb::ios::ui::make_button(@"");
    UILabel*     rndlbl = wb::ios::ui::make_label(@"");
    UITextField* rndout = wb::ios::ui::make_field(@""); rndout.enabled = NO;
    UIButton*    gen    = wb::ios::ui::make_button(@"");
    vc_ = wb::ios::ui::make_stack_vc(@[title, b64lbl, b64in, enc, dec, b64out,
                                       rndlbl, gen, rndout]);
    be.bind_text_oneway(vm.title, wb::ios::ui::view_for(title));
    be.bind_text_oneway(vm.base64Group, wb::ios::ui::view_for(b64lbl));
    be.bind_text_oneway(vm.encodeLabel, wb::ios::ui::view_for(enc));
    be.bind_text_oneway(vm.decodeLabel, wb::ios::ui::view_for(dec));
    be.bind_text_oneway(vm.randomGroup, wb::ios::ui::view_for(rndlbl));
    be.bind_text_oneway(vm.generateLabel, wb::ios::ui::view_for(gen));
    be.bind_text(vm.base64Input, wb::ios::ui::view_for(b64in));
    be.bind_text_oneway(vm.base64Output, wb::ios::ui::view_for(b64out));
    be.bind_command(vm.base64Encode, wb::ios::ui::view_for(enc));
    be.bind_command(vm.base64Decode, wb::ios::ui::view_for(dec));
    be.bind_command(vm.genRandom, wb::ios::ui::view_for(gen));
    be.bind_text_oneway(vm.randomOutput, wb::ios::ui::view_for(rndout));
}

}  // namespace wb::tools::iosview
