// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityLine.h"
#include "ReachabilityNode.h"
#include <QPainter>

QPolygonF createNormalArrow(const QLineF &line);

ReachabilityLine::ReachabilityLine(
    const QString& label,
    ReachabilityNode* first,
    ReachabilityNode* last,
    QGraphicsItem* parent
)
    : QGraphicsItem(parent)
    , m_first(first)
    , m_last(last)
    , m_label(label)
{
    onLabelChanged();
}

QRectF ReachabilityLine::boundingRect() const { return m_boundingRect; }

void ReachabilityLine::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget
)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(QPen(Qt::black));
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);
    painter->setFont(m_labelFont);
    painter->drawPath(m_path);
    painter->drawText(m_labelPos, m_label);
    painter->setBrush(Qt::black);
    painter->drawPolygon(m_arrow);
}

qreal angleBetween(const QPointF& first, const QPointF& second)
{
    qreal deltaX = second.x() - first.x();
    qreal deltaY = second.y() - first.y();
    qreal rotation = -atan2(deltaX, deltaY);
    return qDegreesToRadians(qRadiansToDegrees(rotation) + 180.0);
}

QPointF jointPoint(const QPointF& first, const QPointF& second, qreal radius = 0.)
{
    qreal angle = angleBetween(first, second) - qDegreesToRadians(90);
    return {first.x() + qCos(angle) * (radius + 1), first.y() + qSin(angle) * (radius + 1)};
}

QPointF getIntersectionF(qreal dx, qreal dy, qreal cx, qreal cy, qreal width, qreal height)
{
    if (qAbs(dy / dx) < height / width) {
        return {cx + (dx > 0 ? width : -width), cy + dy * width / qAbs(dx)};
    }
    return {cx + dx * height / qAbs(dy), cy + (dy > 0 ? height : -height)};
}

QPointF jointPoint(const QRectF& first, const QRectF& second)
{
    qreal w = first.width() / 2.;
    qreal h = first.height() / 2.;
    qreal dx = second.center().x() - first.center().x();
    qreal dy = second.center().y() - first.center().y();
    return dx == 0. && dy == 0.
               ? second.center()
               : getIntersectionF(dx, dy, first.center().x(), first.center().y(), w, h);
}

const QString& ReachabilityLine::label() const { return m_label; }

void ReachabilityLine::setLabel(const QString& label)
{
    if (m_label == label) {
        return;
    }

    m_label = label;
}

void ReachabilityLine::updateLayout()
{
    m_path.clear();
    QRectF firstRect(m_first->mapRectToScene(m_first->boundingRect()));
    QRectF lastRect(m_last->mapRectToScene(m_last->boundingRect()));

    QPointF firstCenterBottom = firstRect.center();
    firstCenterBottom.setY(firstRect.bottom());
    QPointF lastCenterTop = lastRect.center();
    lastCenterTop.setY(lastRect.top());

    QLineF line(
        jointPoint(firstCenterBottom, lastCenterTop),
        jointPoint(lastCenterTop, firstCenterBottom)
    );
    m_path.moveTo(line.p1());
    m_path.lineTo(line.p2());

    m_arrow = createNormalArrow(line);
    m_boundingRect = m_path.boundingRect();
    setPos(m_boundingRect.center());

    QFontMetrics fm(m_labelFont);
    QRect textRect = fm.boundingRect(m_label);
    textRect.moveCenter(line.pointAt(0.5).toPoint());
    m_labelPos = QPointF(textRect.x(), textRect.bottom() - fm.descent());

    m_boundingRect = mapRectFromScene(m_boundingRect);
    m_path = mapFromScene(m_path);
    m_arrow = mapFromScene(m_arrow);
    m_labelPos = mapFromScene(m_labelPos);

    update();
}

void ReachabilityLine::onLabelChanged()
{
    QFontMetrics fm(m_labelFont);
    QRect textRect = fm.boundingRect(m_label);
    textRect.moveCenter(m_path.pointAtPercent(0.5).toPoint());
    m_labelPos = QPointF(textRect.x(), textRect.bottom() - fm.descent());
}

QPolygonF createNormalArrow(const QLineF& line)
{
    static const double arrowSize = 10;
    double angle = std::atan2(line.dy() + 1, -line.dx() + 1);
    QPointF arrowP1 =
        line.p2() + QPointF(sin(angle + M_PI / 3.) * arrowSize, cos(angle + M_PI / 3.) * arrowSize);
    QPointF arrowP2 = line.p2() + QPointF(
                                      sin(angle + M_PI - M_PI / 3.) * arrowSize,
                                      cos(angle + M_PI - M_PI / 3.) * arrowSize
                                  );
    return QPolygonF() << line.p2() << arrowP1 << arrowP2;
}