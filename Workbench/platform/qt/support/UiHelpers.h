#pragma once
//
// UiHelpers — Qt View 层共享的小工具（移植自 Aria showcase 的模式）。
//   view_for(w)        把 QWidget* 包成 aria QtView（生命周期托管）
//   subs_attached_to(w) 挂在 QObject 上的订阅袋，owner 析构即释放
//
#include "aria/adapters/qt6/qt_view.hpp"
#include "aria/binding/binding_engine.hpp"
#include "aria/property.hpp"
#include "aria/subscription.hpp"

#include <QLabel>
#include <QObject>
#include <QString>
#include <QWidget>

#include <string>
#include <vector>

namespace wb::ui {

QLabel* make_title(const QString& text, QWidget* parent = nullptr);
QLabel* make_info(const QString& text, QWidget* parent = nullptr);

aria::adapters::qt6::QtView& view_for(QObject* w);
std::vector<aria::Subscription>& subs_attached_to(QObject* owner);

/// 绑定「用户可编辑、VM 不反向推回」的文本输入框（如 base64/json 输入区）。
/// 只做 View→VM + 一次初始同步，**不**订阅 VM→View，避免每次输入时
/// setPlainText/setText 重置光标（光标跳到最前）。
void bind_editable_text(aria::binding::BindingEngine& be,
                        aria::Property<std::string>& prop,
                        QObject* widget);

}  // namespace wb::ui
