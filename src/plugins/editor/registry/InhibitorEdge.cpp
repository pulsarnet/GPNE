// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "InhibitorEdge.h"
#include "IVertex.h"
#include <editor/model/NetModel.h>

IEdge* InhibitorEdge::create() const { return new InhibitorEdge; }

QPainterPath InhibitorEdge::arrowShape(QLineF line)
{
    static constexpr size_t radius = 5;

    double dx = -line.dx();
    double dy = -line.dy();

    double angle = sqrt(dx * dx + dy * dy);
    dx = dx / angle;
    dy = dy / angle;

    QPainterPath path;
    path.addEllipse(
        QPointF(line.p2().x() + dx * radius, line.p2().y() + dy * radius),
        radius,
        radius
    );
    return path;
}

QList<edge_type> InhibitorEdge::getEdgePairs(ptn::net::PetriNet* m_net)
{
    QList<edge_type> result;
    for (auto& inhibitor : m_net->inhibitor_arcs()) {
        result.append(
            {{inhibitor.place().t, inhibitor.place().id},
             {inhibitor.transition().t, inhibitor.transition().id}}
        );
    }
    return result;
}

void InhibitorEdge::submitData()
{
    if (!model()) {
        //qWarning() << "InhibitorEdge::restoreData() Model is NULL";
        return;
    }

    // TODO: rework to general implementation
    auto p = ptn::net::vertex::VertexIndex{place()->vertexType(), place()->id()};
    auto t = ptn::net::vertex::VertexIndex{transition()->vertexType(), transition()->id()};

    if (directions().testFlag(PlaceToTransition)) {
        auto arc = model()->net()->inhibitor_arc(p, t);
        if (!arc) {
            model()->net()->add_inhibitor(p, t);
        }
    }
}

void InhibitorEdge::restoreData()
{
    if (!model()) {
        //qWarning() << "InhibitorEdge::restoreData() Model is NULL";
        return;
    }

    // TODO: rework to general implementation
    auto p = ptn::net::vertex::VertexIndex{place()->vertexType(), place()->id()};
    auto t = ptn::net::vertex::VertexIndex{transition()->vertexType(), transition()->id()};

    if (directions().testFlag(PlaceToTransition)) {
        auto arc = model()->net()->inhibitor_arc(p, t);
        if (!arc) {
            qWarning() << "Edge not found";
        }
    }
}

void InhibitorEdge::removeData()
{
    if (!model()) {
        //qWarning() << "InhibitorEdge::restoreData() Model is NULL";
        return;
    }

    // TODO: rework to general implementation
    auto p = ptn::net::vertex::VertexIndex{place()->vertexType(), place()->id()};
    auto t = ptn::net::vertex::VertexIndex{transition()->vertexType(), transition()->id()};

    if (directions().testFlag(PlaceToTransition)) {
        model()->net()->remove_inhibitor(p, t);
    }
}
