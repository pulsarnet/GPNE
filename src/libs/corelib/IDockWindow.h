// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IDOCKWINDOW_H
#define IDOCKWINDOW_H

#include <QObject>

class QToolBar;

class IDockWindow : public QObject {
    Q_OBJECT

public:

    virtual ~IDockWindow() = default;

    virtual QString title() = 0;

    virtual QIcon icon() = 0;

    virtual QToolBar* toolbar() = 0;

    virtual QWidget* widget() = 0;

};



#endif //IDOCKWINDOW_H
