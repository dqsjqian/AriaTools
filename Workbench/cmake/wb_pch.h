// Precompiled header for Workbench Qt builds.
// Include heavy Qt headers here once; all Qt targets reuse the compiled form.
// Enable via -DWORKBENCH_ENABLE_PCH=ON (default ON for Qt builds).
#pragma once

// Qt Core
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QModelIndex>
#include <QAbstractListModel>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QDateTime>
#include <QUrl>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Qt GUI
#include <QGuiApplication>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QFont>
#include <QColor>
#include <QPalette>

// Qt Widgets
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QTabWidget>
#include <QSplitter>
#include <QScrollArea>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressDialog>
#include <QSystemTrayIcon>

// Qt Quick (if used)
#ifdef WORKBENCH_QT_QUICK
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#endif

// Standard library heavy headers
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <optional>
#include <variant>
