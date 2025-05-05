// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_REACHABILITY_LINE_H
#define GPNE_REACHABILITY_LINE_H

#include <QGraphicsItem>
#include <QFont>

class ReachabilityNode;

class ReachabilityLine : public QGraphicsItem {

public:
    explicit ReachabilityLine(
        const QString& label,
        ReachabilityNode* first,
        ReachabilityNode* last,
        QGraphicsItem* parent = nullptr
    );

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr
    ) override;

    ReachabilityNode* first() const { return m_first; }

    ReachabilityNode* last() const { return m_last; }

    const QString& label() const;
    void setLabel(const QString& label);

    void updateLayout();

protected:

    void onLabelChanged();

private:
    ReachabilityNode* m_first;
    ReachabilityNode* m_last;

    QPainterPath m_path;
    QPolygonF m_arrow;
    QString m_label;
    QFont m_labelFont;
    QPointF m_labelPos;
    QRectF m_boundingRect;
};

#endif // GPNE_REACHABILITY_LINE_H
