// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef XMLSETTINGSFORMAT_H
#define XMLSETTINGSFORMAT_H
#include <QSettings>

QSettings::Format xmlFormat();

bool readXmlFile( QIODevice& device, QSettings::SettingsMap& map );

bool writeXmlFile( QIODevice& device, const QSettings::SettingsMap& map );

#endif //XMLSETTINGSFORMAT_H
