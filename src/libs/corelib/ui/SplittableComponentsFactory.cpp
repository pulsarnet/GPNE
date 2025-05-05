// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SplittableComponentsFactory.h"
#include <DockAreaTitleBar.h>

using namespace ads;

CDockAreaTitleBar*
SplittableComponentsFactory::createDockAreaTitleBar(ads::CDockAreaWidget* dockArea) const
{
    auto titleBar = new CDockAreaTitleBar(dockArea);
    return titleBar;
}
