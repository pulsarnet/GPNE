// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Transition.h"
#include <ptn/simulation.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>

#include "../GraphicsScene.h"

Transition::Transition() { updateRect(); }

Transition::~Transition() = default;

IVertex* Transition::create() const { return new Transition; }

IVertex* Transition::fromJson(const QJsonObject& object) const
{
    auto* super = dynamic_cast<Transition*>(IVertex::fromJson(object));
    const QJsonValue rotatedValue = object["rotated"];

    if (!rotatedValue.isBool()) {
        qWarning() << "'rotated' field: expecting bool";
        return nullptr;
    }

    super->m_rotated = rotatedValue.toBool();
    return super;
}

QJsonObject Transition::toJson() const
{
    QJsonObject result = ITransition::toJson();
    result["rotated"] = m_rotated;
    return result;
}

QRectF Transition::boundingRect() const { return m_rect.adjusted(-6, -6, 6, 6); }

void Transition::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipRect(boundingRect());

    if (isSelected()) {
        painter->save();
        painter->setPen(QPen(Qt::darkGreen, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        painter->setOpacity(0.4);

        painter->drawRect(m_rect.adjusted(-2, -2, 2, 2));
        painter->restore();
    }

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::black);
    painter->drawRect(m_rect);
    painter->restore();
}

QPointF getIntersection(qreal dx, qreal dy, qreal cx, qreal cy, qreal width, qreal height)
{
    if (qAbs(dy / dx) < height / width) {
        return {cx + (dx > 0 ? width : -width), cy + dy * width / qAbs(dx)};
    }
    return {cx + dx * height / qAbs(dy), cy + (dy > 0 ? height : -height)};
}

QPointF Transition::jointPoint(const QPointF& to) const
{
    qreal w = m_rect.width() / 2.;
    qreal h = m_rect.height() / 2.;

    qreal xPosy = to.x();
    qreal yPosy = to.y();

    qreal dx = xPosy - center().x();
    qreal dy = yPosy - center().y();

    qreal cx = center().x();
    qreal cy = center().y();

    auto intersection = dx == 0. && dy == 0. ? to : getIntersection(dx, dy, cx, cy, w, h);

    return intersection;
}

QPointF Transition::center() const { return this->scenePos(); }

void Transition::submitData()
{
    if (!model()) {
        qWarning() << "Transition::restoreData() Model is NULL";
        return;
    }

    if (id() == 0) {
        setId(model()->net()->add_transition());
    } else {
        auto place = model()->net()->transition(id());
        if (!place) {
            model()->net()->insert_transition(id());
        }
    }
}

void Transition::restoreData()
{
    if (!model()) {
        qWarning() << "Transition::restoreData() Model is NULL";
        return;
    }

    auto transition = model()->net()->transition(id());
    if (!transition) {
        qWarning() << "Place with " << id() << "not found in model";
    }
}

void Transition::removeData()
{
    if (model()) {
        model()->net()->remove_transition(id());
    }
}

QList<IVertex*> Transition::getItemsFromNet(ptn::net::PetriNet* m_net)
{
    QList<IVertex*> result;
    for (auto transitions = m_net->transitions(); auto transition : transitions) {
        auto vertex = create();
        vertex->setId(transition);
        result.push_back(vertex);
    }
    return result;
}

void Transition::setRotated(bool rotated)
{
    if (rotated == m_rotated) {
        return;
    }

    m_rotated = rotated;
    updateRect();
    emitDataChanged();
}

void Transition::updateRect()
{
    prepareGeometryChange();
    m_rect = !m_rotated ? QRectF(-10, -30, 20, 60) : QRectF(-30, -10, 60, 20);
    updateLabelPosition();
    updateEdges();
}
