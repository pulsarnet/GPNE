// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROPERTYMAP_H
#define PROPERTYMAP_H
#include <map>
#include <QObject>

class QtProperty;

class PropertyMap {

public:
    explicit PropertyMap() = default;

    bool addProperty(QtProperty*, QLatin1StringView);

    QtProperty* property(QLatin1StringView) const;
    QLatin1StringView id(QtProperty*) const;

    void removeProperty(QtProperty*);
    void removeProperty(QLatin1StringView);

private:
    std::map<QtProperty*, QLatin1String> m_propertyToId;
    std::map<QLatin1String, QtProperty*> m_idToProperty;
};

#endif // PROPERTYMAP_H
