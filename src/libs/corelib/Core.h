// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <QObject>

class Core : public QObject {
    Q_OBJECT

public:
    static void initialize();

private:

    static void startApplication();

};

#endif //COREPLUGIN_H
