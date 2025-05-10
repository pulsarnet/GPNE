// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "PropertyMap.h"

/**
* @brief Adds a new property-identifier mapping
* @param property Pointer to QtProperty to add
* @param id String identifier for the property
* @return true if mapping was added successfully, false if either property or id already exists
*/
bool PropertyMap::addProperty(QtProperty* property, QLatin1StringView id)
{
    if (!property || id.isEmpty()) {
        return false;
    }

    auto [it, success] = m_properties.insert({property, id});
    return success;
}

/**
* @brief Looks up a property by its identifier
* @param id The identifier to search for
* @return Pointer to the corresponding QtProperty, or nullptr if not found
*/
QtProperty* PropertyMap::property(QLatin1StringView id) const
{
    const auto& idIndex = m_properties.get<ById>();
    auto it = idIndex.find(id);

    return it != idIndex.end() ? it->property : nullptr;
}

/**
* @brief Looks up an identifier by its property pointer
* @param property The property pointer to search for
* @return The corresponding identifier, or empty QLatin1StringView if not found
*/
QLatin1StringView PropertyMap::id(QtProperty* property) const
{
    const auto& propertyIndex = m_properties.get<ByProperty>();
    auto it = propertyIndex.find(property);
    return it != propertyIndex.end() ? it->id : QLatin1StringView();
}

/**
* @brief Removes a mapping by property pointer
* @param property The property pointer to remove
*/
void PropertyMap::removeProperty(QtProperty* property)
{
    auto& propertyIndex = m_properties.get<ByProperty>();
    propertyIndex.erase(property);
}

/**
* @brief Removes a mapping by identifier
* @param id The identifier to remove
*/
void PropertyMap::removeProperty(QLatin1StringView id)
{
    auto& idIndex = m_properties.get<ById>();
    idIndex.erase(id);
}
