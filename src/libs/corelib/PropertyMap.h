// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef PROPERTYMAP_H
#define PROPERTYMAP_H

#include <QLatin1StringView>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

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
    struct Entry {
        QtProperty* property;
        QLatin1StringView id;
    };

    struct ByProperty {};
    struct ById {};

    using Container = boost::multi_index_container<
        Entry,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<ByProperty>,
                boost::multi_index::member<Entry, QtProperty*, &Entry::property>
            >,
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<ById>,
                boost::multi_index::member<Entry, QLatin1StringView, &Entry::id>
            >
        >
    >;

    Container m_properties;
};

#endif // PROPERTYMAP_H
