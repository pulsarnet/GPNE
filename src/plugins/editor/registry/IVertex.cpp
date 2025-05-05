// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "IVertex.h"
#include <QApplication>
#include "IEdge.h"
#include <QJsonObject>
#include <QJsonValue>
#include <editor/model/NetModel.h>

void IVertex::setId(usize id)
{
    m_id = id;
    updateLabel();
}

IVertex* IVertex::fromJson(const QJsonObject& object) const
{
    const QJsonValue idValue = object["id"];
    const QJsonValue xValue = object["x"];
    const QJsonValue yValue = object["y"];

    if (!idValue.isDouble()) {
        qWarning() << "'id' field: expecting integer";
        return nullptr;
    }

    if (!xValue.isDouble()) {
        qWarning() << "'x' field: expecting double";
        return nullptr;
    }

    if (!yValue.isDouble()) {
        qWarning() << "'y' field: expecting double";
        return nullptr;
    }

    IVertex* vertex = create();
    vertex->setId((usize)idValue.toInteger());
    vertex->setPos(xValue.toDouble(), yValue.toDouble());
    return vertex;
}

QJsonObject IVertex::toJson() const
{
    QJsonObject result;
    result["id"] = QJsonValue::fromVariant((int)m_id);
    result["x"] = QJsonValue::fromVariant(x());
    result["y"] = QJsonValue::fromVariant(y());
    return result;
}

void IVertex::addEdge(IEdge* edge) { m_edges.insert(edge); }

void IVertex::removeEdge(IEdge* edge) { m_edges.remove(edge); }

QVariant IVertex::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (scene() && change == ItemPositionHasChanged) { //|| change == ItemSceneHasChanged
        // updateLabelPosition();

        for (auto edge : m_edges) {
            edge->updatePosition();
        }

        emitDataChanged();
    } else if (change == ItemPositionChange && scene()) {
        auto newPosition = value.toPointF();
        if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
            qreal gridSize = 50.;
            qreal xV = round(newPosition.x() / gridSize) * gridSize;
            qreal yV = round(newPosition.y() / gridSize) * gridSize;

            return QPointF(xV, yV);
        }
    }

    return PetriObject::itemChange(change, value);
}

void IVertex::emitDataChanged()
{
    if (model()) {
        emit model()->vertexChanged(this);
    }
    update();
}

IVertex::~IVertex() {}

void IVertex::updateEdges()
{
    for (auto edge : m_edges) {
        edge->updatePosition();
    }
}
