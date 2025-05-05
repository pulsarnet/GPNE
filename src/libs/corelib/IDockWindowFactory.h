// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef IDOCKWINDOWFACTORY_H
#define IDOCKWINDOWFACTORY_H

class IDockWindow;
class Project;

class IDockWindowFactory {

public:

    virtual ~IDockWindowFactory() = default;

    virtual IDockWindow* createDockWindow(Project*) = 0;

};


#endif //IDOCKWINDOWFACTORY_H
