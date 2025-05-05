// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifdef Q_OS_WIN
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "ntdll.lib")
#endif

#define QT_MESSAGELOGCONTEXT

#include <corelib/ui/SplittableComponentsFactory.h>
#include <DockComponentsFactory.h>
#include <DockManager.h>
#include <corelib/Core.h>
#include <corelib/XmlSettingsFormat.h>
#include <plugins/editor/registry/DirectedEdge.h>
#include <plugins/editor/registry/IEdge.h>
#include <plugins/editor/registry/InhibitorEdge.h>
#include <plugins/editor/registry/Place.h>
#include <plugins/editor/registry/Registry.h>
#include <plugins/editor/registry/Transition.h>
#include <ptn/logger.h>
#include <QApplication>
#include <qthreadpool.h>
#include <QTranslator>
#include <QFile>

#ifdef USE_CRASHPAD
#include "crashpad.h"
#endif

#include <rfl/xml.hpp>
#include <QStyleFactory>

void customMessageOutput(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    namespace logger = ptn::logger;
    auto utf8LocalMsg = msg.toUtf8();
    const char* localMsg = utf8LocalMsg.constData();
    const char* function = ctx.function;
    const char* filename = ctx.file;
    int line = ctx.line;
    switch (type) {
    case QtDebugMsg:
        logger::debug(localMsg, function, filename, line);
        break;
    case QtInfoMsg:
        logger::info(localMsg, function, filename, line);
        break;
    case QtWarningMsg:
        logger::warn(localMsg, function, filename, line);
        break;
    case QtCriticalMsg:
        logger::error(localMsg, function, filename, line);
        break;
    case QtFatalMsg:
        logger::error(localMsg, function, filename, line);
        abort();
    }
}

struct Person {
    std::string name;
};

int main(int argc, char** argv)
{
    // qInstallMessageHandler(customMessageOutput);
    //
    // ptn::logger::init();

    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHideDisabledButtons, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::HideSingleCentralWidgetTitleBar, true);

    ads::CDockComponentsFactory::setFactory(new SplittableComponentsFactory);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("GPNE");

    QTranslator translator;
    if (translator.load(QLocale(), "gpne", "_", ":/i18n")) {
        QCoreApplication::installTranslator(&translator);
    }

    QApplication::setApplicationDisplayName(
        translator.translate("main", "GPNE - Graphical Petri Net Editor")
    );

#ifdef Q_OS_MACOS
    auto path_string = app.applicationDirPath().toStdString();
#ifdef MACOS_BUNDLE
    std::filesystem::path path(path_string);
    path = path.parent_path().append("Frameworks");
    setenv("GVBINDIR", path.c_str(), 1);
#elifdef MACOS_GVBINDIR
    setenv("GVBINDIR", MACOS_GVBINDIR, 1);
#endif
#endif

#ifdef WIN32
    auto palette = QApplication::palette();
    // Light theme
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(0xf0, 0xf0, 0xf0));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(0x42, 0x85, 0xf4));
    palette.setColor(QPalette::Highlight, QColor(0x42, 0x85, 0xf4));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Base, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Window, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Light, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Midlight, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Dark, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Mid, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Shadow, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(0x80, 0x80, 0x80));
    palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(0x80, 0x80, 0x80));
    QApplication::setPalette(palette);
#endif

    Registry::registerVertex(new Place);
    Registry::registerVertex(new Transition);

    Registry::registerEdge(new DirectedEdge);
    Registry::registerEdge(new InhibitorEdge);

    // Settings INI
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QFile qss(":/styles/style.qss");
    qss.open(QIODeviceBase::ReadOnly);

    QTextStream stream(&qss);
    auto qss_text = stream.readAll();

    app.setStyleSheet(qss_text);

    Core::initialize();

#ifdef USE_CRASHPAD
    if (!startCrashHandler("", L".\\crashpad_handler.exe", L"crashes")) {
        qDebug() << "Failed to start crash handler";
    }
#endif

    int ret = 0;
    try {
        ret = app.exec();
    } catch (const std::exception& e) {
        std::cout << "Exception:" << e.what();
        ret = 1;
    } catch (...) {
        std::cout << "Unknown exception";
        ret = 1;
    }

    return ret;
}
