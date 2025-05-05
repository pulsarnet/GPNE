// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "XmlSettingsFormat.h"
#include <QXmlStreamReader>

static QSettings::Format xml = QSettings::registerFormat("xml", readXmlFile, writeXmlFile);;

QSettings::Format xmlFormat() {
    return xml;
}


bool readXmlFile(QIODevice &device, QMap<QString, QVariant> &map)
{
    if (!device.isOpen() && !device.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader reader(&device);
    map.clear();

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == QStringLiteral("settings")) {
            while (!(reader.isEndElement() && (reader.name() == QStringLiteral("settings"))) && !reader.atEnd()) {
                reader.readNext();
                if (reader.isStartElement() && reader.name() == QStringLiteral("entry")) {
                    const auto key = reader.attributes().value(QStringLiteral("key")).toString();
                    const auto value = reader.attributes().value(QStringLiteral("value")).toString();
                    map.insert(key, value);
                }
            }
        }
    }

    return !reader.hasError();
}

bool writeXmlFile(QIODevice &device, const QMap<QString, QVariant> &map)
{
    if (!device.isOpen() && !device.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamWriter writer(&device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QStringLiteral("settings"));

    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        writer.writeEmptyElement(QStringLiteral("entry"));
        writer.writeAttribute(QStringLiteral("key"), it.key());
        writer.writeAttribute(QStringLiteral("value"), it.value().toString());
    }

    writer.writeEndElement(); // settings
    writer.writeEndDocument();

    return true;
}
