// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "IEdge.h"
#include <QJsonArray>
#include <QPainter>
#include <QCursor>
#include "IVertex.h"

bool IEdge::start(IVertex* vertex)
{
    if (m_directions.toInt() != 0 || begin()) {
        return false;
    }

    if (vertex->classId() == IVertex::Place && allowDirection(PlaceToTransition)) {
        m_place = vertex;
        m_directions.setFlag(PlaceToTransition, true);
    } else if (vertex->classId() == IVertex::Transition && allowDirection(TransitionToPlace)) {
        m_transition = vertex;
        m_directions.setFlag(TransitionToPlace, true);
    } else {
        return false;
    }

    vertex->addEdge(this);
    return true;
}

bool IEdge::finish(IVertex* vertex)
{
    if (m_directions.toInt() == 0 || (m_place && m_transition)) {
        return false;
    }

    if (m_directions.testFlag(PlaceToTransition) && vertex->classId() == IVertex::Transition) {
        m_transition = vertex;
    } else if (m_directions.testFlag(TransitionToPlace) && vertex->classId() == IVertex::Place) {
        m_place = vertex;
    } else {
        return false;
    }

    vertex->addEdge(this);
    updatePosition();
    return true;
}

IVertex* IEdge::begin() const
{
    if (m_directions.testFlag(PlaceToTransition)) {
        return m_place;
    }
    return m_transition;
}

IVertex* IEdge::end() const
{
    if (m_directions.testFlag(PlaceToTransition)) {
        return m_transition;
    }
    return m_place;
}

void IEdge::setDirections(Directions directions)
{
    if (directions.toInt() == 0 || m_directions == directions) {
        return;
    }

    if (directions.testFlag(PlaceToTransition) && !allowDirection(PlaceToTransition)) {
        return;
    }

    if (directions.testFlag(TransitionToPlace) && !allowDirection(TransitionToPlace)) {
        return;
    }

    m_directions = directions;
    updatePosition();
    onWeightChanged();
    emitDataChanged(); // update model
}

bool IEdge::isBidirectional() const
{
    return m_directions.testFlags(PlaceToTransition | TransitionToPlace);
}

void IEdge::setWeight(uint32_t newWeight, Direction direction)
{
    if (newWeight == 0 || !allowDirection(direction)) {
        return;
    }

    if (newWeight == m_weight[direction == PlaceToTransition ? 0 : 1]) {
        return;
    }

    m_weight[direction == PlaceToTransition ? 0 : 1] = newWeight;
    onWeightChanged();
    emitDataChanged(); // update model
}

void IEdge::onWeightChanged()
{
    submitData();
    updateLabel();
    update(); // redraw
}

QPainterPath IEdge::shape() const
{
    QPainterPath path;
    if (line() == QLineF()) {
        return path;
    }

    path.moveTo(line().p1());
    path.lineTo(line().p2());

    QPainterPathStroker stroker;
    stroker.setWidth(10);
    stroker.setCapStyle(Qt::SquareCap);
    return stroker.createStroke(path);
}

QRectF IEdge::boundingRect() const { return shape().boundingRect().adjusted(-15., -15, 15, 15); }

void IEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipRect(boundingRect());
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

    if (isSelected()) {
        painter->save();
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::darkGreen, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        painter->setOpacity(0.4);
        painter->drawLine(m_selectionLine);
        painter->restore();
    }

    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    painter->drawLine(m_drawLine);

    painter->setBrush(Qt::black);
    painter->setPen(Qt::NoPen);

    if (m_directions.toInt() == 3) {
        painter->drawPath(m_arrowShape[0]);
        painter->drawPath(m_arrowShape[1]);
    } else if (m_directions.testFlag(PlaceToTransition)) {
        painter->drawPath(m_arrowShape[0]);
    } else if (m_directions.testFlag(TransitionToPlace)) {
        painter->drawPath(m_arrowShape[1]);
    }

    painter->restore();

    if (!m_text.isEmpty()) {
        painter->save();
        painter->setTransform(m_textTransform * painter->transform());
        QFontMetricsF metrics(painter->font());
        QSizeF size = metrics.size(0, m_text);
        painter->drawText(-size.width() / 2, -2, m_text);
        painter->restore();
    }
}

QVariant IEdge::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSceneHasChanged && scene()) {
        updatePosition();
    } else if (change == ItemSelectedHasChanged && scene()) {
        updateDrawLine();
    }
    return QGraphicsLineItem::itemChange(change, value);
}

void IEdge::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::SizeAllCursor);
    QGraphicsLineItem::hoverEnterEvent(event);
}

void IEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::ArrowCursor);
    QGraphicsLineItem::hoverLeaveEvent(event);
}

void IEdge::moveTo(QPointF point)
{
    if (m_place) {
        setLine({m_place->jointPoint(point), point});
    }
    if (m_transition) {
        setLine({point, m_transition->jointPoint(point)});
    }
}

void IEdge::updatePosition()
{
    if (!scene()) {
        return;
    }

    if (m_place && m_transition) {
        QLineF newLine(
            m_place->jointPoint(m_transition->scenePos()),
            m_transition->jointPoint(m_place->scenePos())
        );
        setLine(newLine);
    }

    if (m_place) {
        m_arrowShape[0] = arrowShape(line());
    }

    if (m_transition) {
        m_arrowShape[1] = arrowShape(QLineF(line().p2(), line().p1()));
    }

    updateDrawLine();
    updateLabel();
    update();
}

void IEdge::emitDataChanged()
{
    if (model()) {
        emit model()->edgeChanged(this);
    }
}

bool IEdge::loadFromJson(const QJsonObject& object)
{
    const QJsonValue weights = object["weights"];
    if (!weights.isArray()) {
        qWarning() << "Err";
        return false;
    }

    const QJsonArray weightsArray = weights.toArray();
    if (weightsArray.size() != 2) {
        qWarning() << "Err";
        return false;
    }

    const QJsonValue pTot = weightsArray.first();
    const QJsonValue tTop = weightsArray.last();

    if (!pTot.isDouble() || !tTop.isDouble()) {
        qWarning() << "Err";
        return false;
    }

    m_weight[0] = pTot.toInt();
    m_weight[1] = tTop.toInt();

    // restore direction
    const QJsonValue directions = object["directions"];
    if (!directions.isDouble()) {
        qWarning() << "Err";
        return false;
    }
    m_directions = Directions::fromInt(directions.toInt());

    onWeightChanged();

    return true;
}

QJsonObject IEdge::toJson() const
{
    QJsonObject object;
    QJsonObject place;
    place["id"] = (double)m_place->id();
    place["type"] = (QLatin1String)m_place->typeId();

    QJsonObject transition;
    transition["id"] = (double)m_transition->id();
    transition["type"] = (QLatin1String)m_transition->typeId();

    QJsonObject data;
    QJsonArray weights;
    weights.append((qint64)m_weight[0]);
    weights.append((qint64)m_weight[1]);
    data["weights"] = QJsonArray(weights);
    data["directions"] = (int)m_directions.toInt();

    object["place"] = place;
    object["transition"] = transition;
    object["data"] = data;
    return object;
}

IEdge::~IEdge()
{
    if (m_place) {
        m_place->removeEdge(this);
    }

    if (m_transition) {
        m_transition->removeEdge(this);
    }
}

IEdge::IEdge()
{
    setFlag(ItemIsSelectable, true);
    setAcceptHoverEvents(true);

    setZValue(-1);
}

void IEdge::updateDrawLine()
{
    m_drawLine = line();
    m_selectionLine = line();

    // Calculate line length
    const qreal x0 = m_drawLine.p1().x();
    const qreal y0 = m_drawLine.p1().y();
    const qreal x1 = m_drawLine.p2().x();
    const qreal y1 = m_drawLine.p2().y();

    qreal lineLength = sqrt(pow(x1 - x0, 2.) + pow(y1 - y0, 2.));
    qreal ratio = 2. / lineLength;
    qreal selectionArrowRatio = 6. / lineLength;
    if (lineLength == 0) {
        return;
    }

    if (directions().testFlag(PlaceToTransition)) {
        m_drawLine.setP2({(1. - ratio) * x1 + ratio * x0, (1. - ratio) * y1 + ratio * y0});
        m_selectionLine.setP2(
            {(1. - selectionArrowRatio) * x1 + selectionArrowRatio * x0,
             (1. - selectionArrowRatio) * y1 + selectionArrowRatio * y0}
        );
    }

    if (directions().testFlag(TransitionToPlace)) {
        m_drawLine.setP1({(1. - ratio) * x0 + ratio * x1, (1. - ratio) * y0 + ratio * y1});
        m_selectionLine.setP1(
            {(1. - selectionArrowRatio) * x0 + selectionArrowRatio * x1,
             (1. - selectionArrowRatio) * y0 + selectionArrowRatio * y1}
        );
    }
}

void IEdge::updateLabel()
{
    m_textTransform.reset();
    double angle = std::atan2(-line().dy(), line().dx()) * 180 / M_PI;
    m_textTransform.translate(line().center().x(), line().center().y());
    m_textTransform.rotate(-angle);
    if (std::abs(angle) > 90) {
        m_textTransform.rotate(180);
    }

    bool bidirectional = m_directions.testFlags(PlaceToTransition | TransitionToPlace);
    if (bidirectional && (m_weight[0] > 1 || m_weight[1] > 1)) {
        if (std::abs(angle) > 90) {
            m_text = QString("<- %1 | %2 ->").arg(m_weight[0]).arg(m_weight[1]);
        } else {
            m_text = QString("<- %1 | %2 ->").arg(m_weight[1]).arg(m_weight[0]);
        }
    } else if (!bidirectional && (weight(PlaceToTransition) > 1 || weight(TransitionToPlace) > 1)) {
        m_text = QString("%1").arg(weight(
            m_directions.testFlags(PlaceToTransition) ? PlaceToTransition : TransitionToPlace
        ));
    } else {
        m_text.clear();
    }
}
