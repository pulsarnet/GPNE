// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef INHIBITOREDGE_H
#define INHIBITOREDGE_H

#include "IEdge.h"
#include <QByteArray>

class InhibitorEdge : public IEdge {
public:
    QByteArray typeId() const override { return "Inhibitor"; }

    QIcon icon() const override { return QIcon(":/images/tools/inhibitor.svg"); }

    IEdge* create() const override;

    bool allowDirection(Direction direction) const override
    {
        return direction == PlaceToTransition;
    }

    bool hasWeight() const override { return false; }

    QPainterPath arrowShape(QLineF line) override;

    void submitData() override;
    void restoreData() override;
    void removeData() override;

    QList<edge_type> getEdgePairs(ptn::net::PetriNet* m_net) override;
};

#endif // INHIBITOREDGE_H
