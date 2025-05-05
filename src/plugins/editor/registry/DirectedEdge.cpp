// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "DirectedEdge.h"
#include "IVertex.h"
#include <editor/model/NetModel.h>

IEdge* DirectedEdge::create() const { return new DirectedEdge; }

QPainterPath DirectedEdge::arrowShape(QLineF line)
{
    static const double arrowSize = 10;
    double angle = std::atan2(line.dy() + 1, -line.dx() + 1);
    QPointF arrowP1 =
        line.p2() + QPointF(sin(angle + M_PI / 3.) * arrowSize, cos(angle + M_PI / 3.) * arrowSize);
    QPointF arrowP2 = line.p2() + QPointF(
                                      sin(angle + M_PI - M_PI / 3.) * arrowSize,
                                      cos(angle + M_PI - M_PI / 3.) * arrowSize
                                  );
    QPolygonF arrow;
    arrow << line.p2() << arrowP1 << arrowP2;

    QPainterPath path;
    path.addPolygon(arrow);
    return path;
}

QList<edge_type> DirectedEdge::getEdgePairs(ptn::net::PetriNet* m_net)
{
    QList<edge_type> result;
    for (auto& directed : m_net->directed_arcs()) {
        result.append({{directed.from().t, directed.from().id}, {directed.to().t, directed.to().id}}
        );
    }
    return result;
}

void DirectedEdge::submitData()
{
    if (!model()) {
        //qWarning() << "DirectedEdge::submitData() Model is NULL";
        return;
    }

    // TODO: rework to general implementation
    auto p = ptn::net::vertex::VertexIndex{place()->vertexType(), place()->id()};
    auto t = ptn::net::vertex::VertexIndex{transition()->vertexType(), transition()->id()};

    if (directions().testFlag(PlaceToTransition)) {
        auto arc = model()->net()->directed_arc(p, t);
        if (!arc) {
            model()->net()->add_directed(p, t, weight(PlaceToTransition));
        } else {
            arc->set_weight(weight(PlaceToTransition));
        }
    } else {
        model()->net()->remove_directed(p, t);
    }

    if (directions().testFlag(TransitionToPlace)) {
        auto arc = model()->net()->directed_arc(t, p);
        if (!arc) {
            model()->net()->add_directed(t, p, weight(TransitionToPlace));
        } else {
            arc->set_weight(weight(TransitionToPlace));
        }
    } else {
        model()->net()->remove_directed(t, p);
    }
}

void DirectedEdge::restoreData()
{
    if (!model()) {
        //qWarning() << "DirectedEdge::restoreData() Model is NULL";
        return;
    }

    // TODO: rework to general implementation
    auto p = ptn::net::vertex::VertexIndex{place()->vertexType(), place()->id()};
    auto t = ptn::net::vertex::VertexIndex{transition()->vertexType(), transition()->id()};

    if (directions().testFlag(PlaceToTransition)) {
        auto arc = model()->net()->directed_arc(p, t);
        if (!arc) {
            qWarning() << "Edge not found";
        } else {
            setWeight(arc->weight(), PlaceToTransition);
        }
    }

    if (directions().testFlag(TransitionToPlace)) {
        auto arc = model()->net()->directed_arc(t, p);
        if (!arc) {
            qWarning() << "Edge not found";
        } else {
            setWeight(arc->weight(), TransitionToPlace);
        }
    }
}

void DirectedEdge::removeData()
{
    if (!model()) {
        //qWarning() << "DirectedEdge::removeData() Model is NULL";
        return;
    }

    // TODO: rework to general implementation
    auto p = ptn::net::vertex::VertexIndex{place()->vertexType(), place()->id()};
    auto t = ptn::net::vertex::VertexIndex{transition()->vertexType(), transition()->id()};

    if (directions().testFlag(PlaceToTransition)) {
        model()->net()->remove_directed(p, t);
    }

    if (directions().testFlag(TransitionToPlace)) {
        model()->net()->remove_directed(t, p);
    }
}
