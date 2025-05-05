// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SIMULATIONPLUGIN_H
#define SIMULATIONPLUGIN_H

#include <corelib/PluginInterface.h>
#include <QObject>

class IEditor;

class SimulationPlugin : public QObject, public Plugin::PluginInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gpne.plugin.simulation")
    Q_INTERFACES(Plugin::PluginInterface)

public:

    SimulationPlugin() = default;

    void initialize() override;

public slots:

    void onEditorOpened(IEditor* editor);

};



#endif //SIMULATIONPLUGIN_H
