// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef JSONFORMAT_H
#define JSONFORMAT_H

class QJsonObject;
class NetModel;

class JsonFormat {

public:
    explicit JsonFormat() = delete;

    static bool load(NetModel*, const QJsonObject&);
    static void write(NetModel*, QJsonObject&);
};

#endif // JSONFORMAT_H
