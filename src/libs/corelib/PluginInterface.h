// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QObject>

namespace Plugin {
class PluginInterface {
public:
    PluginInterface() = default;
    virtual ~PluginInterface() = default;

    virtual void initialize() = 0;
};
}

Q_DECLARE_INTERFACE(Plugin::PluginInterface, "org.gpne.PluginInterface")

#endif //PLUGININTERFACE_H
