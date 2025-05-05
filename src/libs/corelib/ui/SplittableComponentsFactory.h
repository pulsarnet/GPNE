// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_SPLITTABLECOMPONENTSFACTORY_H
#define GPNE_SPLITTABLECOMPONENTSFACTORY_H

#include <DockComponentsFactory.h>

class SplittableComponentsFactory : public ads::CDockComponentsFactory {
public:
    ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* dockArea) const override;
};

#endif // GPNE_SPLITTABLECOMPONENTSFACTORY_H
