// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include <catch2/catch_test_macros.hpp>
#include <corelib/PropertyMap.h>
#include <qtpropertybrowser.h>
#include <qtpropertymanager.h>

TEST_CASE("PropertyMap")
{
    PropertyMap map;

    QtIntPropertyManager* manager = new QtIntPropertyManager();
    QtProperty* property1 = manager->addProperty("Property1");
    QtProperty* property2 = manager->addProperty("Property2");

    SECTION("Add property")
    {
        REQUIRE(map.addProperty(property1, QLatin1StringView("Property1")));
    }

    SECTION("Reject add property by id")
    {
        REQUIRE(map.addProperty(property1, QLatin1StringView("Property1")));
        REQUIRE_FALSE(map.addProperty(property2, QLatin1StringView("Property1")));
        REQUIRE(map.property(QLatin1String("Property1")) == property1);
        REQUIRE(map.id(property2) == QLatin1StringView());
    }
}
