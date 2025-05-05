// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SYNTHESISPLUGIN_H
#define SYNTHESISPLUGIN_H

#include <corelib/PluginInterface.h>
#include <QHash>
#include <QSet>

class PetriNetDocument;
class IEditor;
class SynthesisDocument;
class QAction;

class SynthesisPlugin : public QObject, public Plugin::PluginInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gpne.plugin.synthesis")
    Q_INTERFACES(Plugin::PluginInterface)

public:

    SynthesisPlugin() = default;

    void initialize();

public slots:

    void onSynthesisAction();
    void onRunDirectedSynthesisAction();
    void onEditorOpened(IEditor* editor);

private:

    QAction* m_synthesisAction;
    QAction* m_runDirectedSynthesisAction;
    QSet<PetriNetDocument*> m_lockedDocuments;
    QHash<PetriNetDocument*, SynthesisDocument*> m_synthesisDocuments;

};

#endif //SYNTHESISPLUGIN_H
