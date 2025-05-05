// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef MATRIXPLUGIN_H
#define MATRIXPLUGIN_H

#include <corelib/PluginInterface.h>

class Project;
class IEditor;

class MatrixPlugin : public QObject, public Plugin::PluginInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gpne.plugin.iomatrix")
    Q_INTERFACES(Plugin::PluginInterface)

public:

    MatrixPlugin() = default;

    void initialize();

public slots:

    void onEditorOpened(IEditor* editor);

    void onMatrixWindowRequested();

};

#endif //MATRIXPLUGIN_H
