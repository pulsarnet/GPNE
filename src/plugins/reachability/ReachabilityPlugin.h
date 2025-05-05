// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef REACHABILITYPLUGIN_H
#define REACHABILITYPLUGIN_H

#include <QSet>
#include <corelib/PluginInterface.h>

class QAction;
class ReachabilityDocument;
class PetriNetDocument;
class IEditor;

class ReachabilityPlugin : public QObject, public Plugin::PluginInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gpne.plugin.reachability")
    Q_INTERFACES(Plugin::PluginInterface)

public:

    ReachabilityPlugin() = default;

    void initialize();

public slots:

    void onEditorOpened(IEditor *editor);
    void onReachabilityTreeRequested();
    void onReloadRequested();
    void onFitInView();

private:

    QSet<PetriNetDocument*> m_lockedDocuments;
    QHash<PetriNetDocument*, ReachabilityDocument*> m_reachabilityDocuments;

    QAction* m_reachabilityAction = nullptr;
    QAction* m_reloadAction = nullptr;
    QAction* m_fitInViewAction = nullptr;
};

#endif //REACHABILITYPLUGIN_H
