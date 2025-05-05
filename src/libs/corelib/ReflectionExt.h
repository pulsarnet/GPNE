// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef REFLECTIONEXT_H
#define REFLECTIONEXT_H

#include <rfl/internal/has_reflector.hpp>

namespace rfl {
    template <>
    struct Reflector<QString> {
        using ReflType = std::string;

        static QString to(const ReflType& str) noexcept {
            return QString::fromStdString(str);
        }

        static ReflType from(const QString& v) {
            return v.toStdString();
        }
    };

    template <>
    struct Reflector<QDateTime> {
        using ReflType = std::string;
        static QDateTime to(const ReflType& str) noexcept {
            return QDateTime::fromString(QString::fromStdString(str), Qt::ISODate);
        }

        static ReflType from(const QDateTime& v) {
            return v.toString(Qt::ISODate).toStdString();
        }
    };
}

#endif //REFLECTIONEXT_H
