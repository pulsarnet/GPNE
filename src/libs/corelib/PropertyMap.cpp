// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "PropertyMap.h"

bool PropertyMap::addProperty(QtProperty* property, QLatin1StringView id)
{
    auto propIt = m_propertyToId.find(property);
    if (propIt != m_propertyToId.end()) {
        return false;
    }

    auto idIt = m_idToProperty.find(id);
    if (idIt != m_idToProperty.end()) {
        return false;
    }

    m_propertyToId.insert({property, id});
    m_idToProperty.insert({id, property});
    return true;
}

QtProperty* PropertyMap::property(QLatin1StringView id) const
{
    auto it = m_idToProperty.find(id);
    if (it == m_idToProperty.end()) {
        return nullptr;
    }

    return it->second;
}

QLatin1StringView PropertyMap::id(QtProperty* prop) const
{
    auto it = m_propertyToId.find(prop);
    if (it == m_propertyToId.end()) {
        return {};
    }

    return it->second;
}

void PropertyMap::removeProperty(QtProperty* prop) { m_propertyToId.erase(prop); }

void PropertyMap::removeProperty(QLatin1StringView id) { m_idToProperty.erase(id); }
