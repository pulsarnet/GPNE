// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Place.h"
#include <ptn/simulation.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>

#include "../GraphicsScene.h"

#define PLACE_RADIUS 25

IVertex* Place::create() const { return new Place(); }

IVertex* Place::fromJson(const QJsonObject& object) const
{
    auto* super = dynamic_cast<Place*>(IVertex::fromJson(object));
    const QJsonValue markersValue = object["markers"];

    if (!markersValue.isDouble()) {
        qWarning() << "'markers' field: expecting integer";
        return nullptr;
    }

    super->m_markers = markersValue.toInt();
    return super;
}

QJsonObject Place::toJson() const
{
    QJsonObject result = IPlace::toJson();
    result["markers"] = (long long)m_markers;
    return result;
}

QPainterPath Place::shape() const
{
    QPainterPath path;
    path.addEllipse({0, 0}, PLACE_RADIUS, PLACE_RADIUS);
    return path;
}

Place::~Place() = default;

QRectF Place::boundingRect() const
{
    return shape().boundingRect().adjusted(-6, -6, 6, 6).normalized();
}

void Place::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    qreal penWidth = 2;

    auto rect = this->boundingRect();

    if (isSelected()) {
        painter->save();
        painter->setPen(QPen(Qt::darkGreen, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        painter->setOpacity(0.4);

        painter->drawEllipse(shape().boundingRect().adjusted(-2, -2, 2, 2));
        painter->restore();
    }

    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(Qt::black, penWidth));
    painter->drawPath(shape());
    // painter->drawEllipse(rect.center(), PLACE_RADIUS - penWidth / 2.,
    // PLACE_RADIUS - penWidth / 2.);
    painter->restore();

    if (m_markers == 1) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));
        painter->drawEllipse(rect.center(), 3., 3.);
        painter->restore();
    } else if (m_markers == 2) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (m_markers == 3) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() + 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() - 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (m_markers == 4) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() + 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x());
        rect.setY(rect.y() - 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 18.);
        rect.setY(rect.y());
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (m_markers == 5) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 12.);
        rect.setY(rect.y() + 12.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 24.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x());
        rect.setY(rect.y() - 24.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 24.);
        rect.setY(rect.y());
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 12.);
        rect.setY(rect.y() + 12.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (m_markers > 5) {
        painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(m_markers));
    }
}

QPointF Place::jointPoint(const QPointF& to) const
{
    qreal angle = this->angleBetween(to);

    qreal x = scenePos().x();
    qreal y = scenePos().y();

    angle = angle - qDegreesToRadians(90);
    qreal xPosy = (x + qCos(angle) * qreal(PLACE_RADIUS + 1));
    qreal yPosy = (y + qSin(angle) * qreal(PLACE_RADIUS + 1));

    return {xPosy, yPosy};
}

QPointF Place::center() const { return scenePos(); }

void Place::submitData()
{
    if (!model()) {
        qWarning() << "Place::restoreData() Model is NULL";
        return;
    }

    if (id() == 0) {
        setId(model()->net()->add_place());
    } else {
        auto place = model()->net()->place(id());
        if (!place) {
            model()->net()->insert_place(id());
        }
    }

    auto place = model()->net()->place(id());
    place->set_markers(m_markers);
}

void Place::restoreData()
{
    if (!model()) {
        qWarning() << "Place::restoreData() Model is NULL";
        return;
    }

    const auto place = model()->net()->place(id());
    if (!place) {
        qWarning() << "Place with " << id() << "not found in model";
        return;
    }

    m_markers = place->markers(); // NOLINT(*-narrowing-conversions)
}

void Place::removeData()
{
    if (model()) {
        model()->net()->remove_place(id());
    }
}

QList<IVertex*> Place::getItemsFromNet(ptn::net::PetriNet* m_net)
{
    QList<IVertex*> result;
    for (auto places = m_net->places(); auto place : places) {
        auto vertex = create();
        vertex->setId(place);
        result.push_back(vertex);
    }
    return result;
}

void Place::setMarkers(usize markers)
{
    if (m_markers == markers) {
        return;
    }

    m_markers = markers;
    submitData();
    emitDataChanged();
}
