#pragma once
//
// UiHelpers — Small shared helpers for the Qt View layer (ported from the Aria showcase pattern).
//   view_for(w)        Wraps a QWidget* into an aria QtView (lifetime managed).
//   subs_attached_to(w) Subscription bag attached to a QObject; released when the owner is destroyed.
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

/// Bind a text input that is "user-editable, VM does not push back" (e.g. base64/json input area).
/// Only does View->VM + one initial sync; does **not** subscribe to VM->View, to avoid on every input
/// setPlainText/setText resetting the cursor (cursor jumps to the beginning).
void bind_editable_text(aria::binding::BindingEngine& be,
                        aria::Property<std::string>& prop,
                        QObject* widget);

}  // namespace wb::ui
