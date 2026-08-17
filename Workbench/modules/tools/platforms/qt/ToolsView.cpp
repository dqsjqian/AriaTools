#include "support/QtViewFactory.h"
#include "support/UiHelpers.h"
#include "viewmodels/ToolsVm.h"

#include "aria/binding/binding_engine.hpp"

#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace wb::tools::qtview {

static void bind_group_title(QGroupBox* box, aria::Property<std::string>& prop,
                             std::vector<aria::Subscription>& subs) {
    box->setTitle(QString::fromStdString(prop.get()));
    subs.push_back(prop.on_changed([box](const std::string& text) {
        box->setTitle(QString::fromStdString(text));
    }));
}

static void bind_label(QLabel* label, aria::Property<std::string>& prop,
                       std::vector<aria::Subscription>& subs) {
    label->setText(QString::fromStdString(prop.get()));
    subs.push_back(prop.on_changed([label](const std::string& text) {
        label->setText(QString::fromStdString(text));
    }));
}

static QGroupBox* build_base64(ToolsVm& vm, aria::binding::BindingEngine& be,
                               std::vector<aria::Subscription>& subs) {
    auto* box = new QGroupBox;
    bind_group_title(box, vm.base64Group, subs);
    auto* layout = new QVBoxLayout(box);
    auto* inputLabel = new QLabel;
    auto* outputLabel = new QLabel;
    auto* input = new QPlainTextEdit;
    auto* output = new QPlainTextEdit;
    auto* encode = new QPushButton;
    auto* decode = new QPushButton;
    auto* buttons = new QHBoxLayout;
    output->setReadOnly(true);
    bind_label(inputLabel, vm.inputLabel, subs);
    bind_label(outputLabel, vm.outputLabel, subs);
    buttons->addWidget(encode);
    buttons->addWidget(decode);
    buttons->addStretch();
    layout->addWidget(inputLabel);
    layout->addWidget(input);
    layout->addLayout(buttons);
    layout->addWidget(outputLabel);
    layout->addWidget(output);
    be.bind_text_oneway(vm.encodeLabel, wb::ui::view_for(encode));
    be.bind_text_oneway(vm.decodeLabel, wb::ui::view_for(decode));
    wb::ui::bind_editable_text(be, vm.base64Input, input);
    be.bind_text_oneway(vm.base64Output, wb::ui::view_for(output));
    be.bind_command(vm.base64Encode, wb::ui::view_for(encode));
    be.bind_command(vm.base64Decode, wb::ui::view_for(decode));
    return box;
}

static QGroupBox* build_random(ToolsVm& vm, aria::binding::BindingEngine& be,
                               std::vector<aria::Subscription>& subs) {
    auto* box = new QGroupBox;
    bind_group_title(box, vm.randomGroup, subs);
    auto* row = new QHBoxLayout(box);
    auto* lengthLabel = new QLabel;
    auto* length = new QSpinBox;
    auto* generate = new QPushButton;
    auto* output = new QLineEdit;
    length->setRange(ToolsVm::kRandomLengthMin, ToolsVm::kRandomLengthMax);
    output->setReadOnly(true);
    bind_label(lengthLabel, vm.lengthLabel, subs);
    row->addWidget(lengthLabel);
    row->addWidget(length);
    row->addWidget(generate);
    row->addWidget(output, 1);
    be.bind_text_oneway(vm.generateLabel, wb::ui::view_for(generate));
    be.bind_int(vm.randomLength, wb::ui::view_for(length));
    be.bind_command(vm.genRandom, wb::ui::view_for(generate));
    be.bind_text_oneway(vm.randomOutput, wb::ui::view_for(output));
    return box;
}

static QGroupBox* build_json(ToolsVm& vm, aria::binding::BindingEngine& be,
                             std::vector<aria::Subscription>& subs) {
    auto* box = new QGroupBox;
    bind_group_title(box, vm.jsonGroup, subs);
    auto* layout = new QVBoxLayout(box);
    auto* input = new QPlainTextEdit;
    auto* output = new QPlainTextEdit;
    auto* status = new QLabel;
    auto* format = new QPushButton;
    auto* minify = new QPushButton;
    auto* buttons = new QHBoxLayout;
    output->setReadOnly(true);
    buttons->addWidget(format);
    buttons->addWidget(minify);
    buttons->addStretch();
    layout->addWidget(input);
    layout->addLayout(buttons);
    layout->addWidget(status);
    layout->addWidget(output);
    be.bind_text_oneway(vm.formatLabel, wb::ui::view_for(format));
    be.bind_text_oneway(vm.minifyLabel, wb::ui::view_for(minify));
    wb::ui::bind_editable_text(be, vm.jsonInput, input);
    be.bind_command(vm.jsonFormat, wb::ui::view_for(format));
    be.bind_command(vm.jsonMinify, wb::ui::view_for(minify));
    be.bind_text_oneway(vm.jsonStatus, wb::ui::view_for(status));
    be.bind_text_oneway(vm.jsonOutput, wb::ui::view_for(output));
    return box;
}

static QGroupBox* build_file_crypto(ToolsVm& vm, aria::binding::BindingEngine& be,
                                    std::vector<aria::Subscription>& subs) {
    auto* box = new QGroupBox;
    bind_group_title(box, vm.fileGroup, subs);
    auto* layout = new QVBoxLayout(box);
    auto* fileRow = new QHBoxLayout;
    auto* filePath = new QLineEdit;
    auto* choose = new QPushButton;
    auto* passphrase = new QLineEdit;
    auto* buttons = new QHBoxLayout;
    auto* encrypt = new QPushButton;
    auto* decrypt = new QPushButton;
    auto* status = new QLabel;
    passphrase->setEchoMode(QLineEdit::Password);
    status->setWordWrap(true);
    fileRow->addWidget(filePath, 1);
    fileRow->addWidget(choose);
    buttons->addWidget(encrypt);
    buttons->addWidget(decrypt);
    buttons->addStretch();
    layout->addLayout(fileRow);
    layout->addWidget(passphrase);
    layout->addLayout(buttons);
    layout->addWidget(status);
    be.bind_text_oneway(vm.chooseFileLabel, wb::ui::view_for(choose));
    be.bind_text_oneway(vm.encryptFileLabel, wb::ui::view_for(encrypt));
    be.bind_text_oneway(vm.decryptFileLabel, wb::ui::view_for(decrypt));
    wb::ui::bind_editable_text(be, vm.filePath, filePath);
    wb::ui::bind_editable_text(be, vm.passphrase, passphrase);
    be.bind_command(vm.encryptFile, wb::ui::view_for(encrypt));
    be.bind_command(vm.decryptFile, wb::ui::view_for(decrypt));
    be.bind_text_oneway(vm.fileStatus, wb::ui::view_for(status));
    QObject::connect(choose, &QPushButton::clicked, [filePath, &vm] {
        const QString path = QFileDialog::getOpenFileName(filePath);
        if (!path.isEmpty()) {
            filePath->setText(path);
            vm.filePath.set(path.toStdString());
        }
    });
    return box;
}

static QWidget* build(ToolsVm& vm, aria::binding::BindingEngine& be) {
    auto* widget = new QWidget;
    auto& subs = wb::ui::subs_attached_to(widget);
    auto* layout = new QVBoxLayout(widget);
    auto* title = wb::ui::make_title("");
    layout->addWidget(title);
    be.bind_text_oneway(vm.title, wb::ui::view_for(title));
    layout->addWidget(build_base64(vm, be, subs));
    layout->addWidget(build_random(vm, be, subs));
    layout->addWidget(build_json(vm, be, subs));
    layout->addWidget(build_file_crypto(vm, be, subs));
    layout->addStretch();
    return widget;
}

}  // namespace wb::tools::qtview

namespace wb::tools {

void register_tools_view() {
    wb::qt::QtViewFactory::instance().register_builder(
        "tools", [](aria::binding::ViewModel& vm, aria::binding::BindingEngine& be) {
            return qtview::build(static_cast<ToolsVm&>(vm), be);
        });
}

}  // namespace wb::tools
