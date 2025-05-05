// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef DIRECTEDEDGE_H
#define DIRECTEDEDGE_H

#include "IEdge.h"

class DirectedEdge : public IEdge {
public:
    QByteArray typeId() const override { return "Directed"; }

    QIcon icon() const override { return QIcon(":/images/tools/connect.svg"); }

    IEdge* create() const override;

    bool allowDirection(Direction direction) const override
    {
        return direction == PlaceToTransition || direction == TransitionToPlace;
    }

    bool hasWeight() const override { return true; }

    QPainterPath arrowShape(QLineF line) override;

    void submitData() override;
    void restoreData() override;
    void removeData() override;

    QList<edge_type> getEdgePairs(ptn::net::PetriNet* m_net) override;
};

#endif // DIRECTEDEDGE_H
