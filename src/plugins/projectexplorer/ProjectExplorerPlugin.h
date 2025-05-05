// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROJECTEXPLORERPLUGIN_H
#define PROJECTEXPLORERPLUGIN_H

#include <corelib/PluginInterface.h>

class QAction;
class IEditor;

class ProjectExplorerPlugin : public QObject, public Plugin::PluginInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gpne.plugin.projectexplorer")
    Q_INTERFACES(Plugin::PluginInterface)

public:

    ProjectExplorerPlugin() = default;

    void initialize() override;

public slots:

    void onEditorOpened(IEditor* editor);

    static void onNewNetAction();
    static void onSaveNetAction();

private:

    QAction* m_newNetAction;
    QAction* m_saveNetAction;

};

#endif //PROJECTEXPLORERPLUGIN_H
