// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef EDITORPLUGIN_H
#define EDITORPLUGIN_H
#include <QSet>
#include <corelib/PluginInterface.h>

class IEditor;
class PetriNetDocument;
class QAction;
class FileRef;

class EditorPlugin : public QObject, public Plugin::PluginInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gpne.plugin.editor")
    Q_INTERFACES(Plugin::PluginInterface)

public:

    EditorPlugin() = default;

    void initialize() override;

public slots:

    void onEditorOpened(IEditor* editor);

private:

    QAction* m_newNetAction;
    QAction* m_saveNetAction;
    QAction* m_netAction;

};

#endif //EDITORPLUGIN_H
