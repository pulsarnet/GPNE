// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_REACHABILITY_NODE_H
#define GPNE_REACHABILITY_NODE_H

#include <ptn/reachability.h>
#include <QFont>
#include <QGraphicsItem>

class Node;

class ReachabilityNode : public QGraphicsItem {

public:
    explicit ReachabilityNode(
        const QList<int32_t>& values,
        const QList<ptn::net::vertex::VertexIndex>& headers,
        QGraphicsItem* parent = nullptr
    );

    [[nodiscard]]
    const QString& text() const;

    Node* drawNode() const;
    void setDrawNode(Node* node);

    const QString& label();
    void setLabel(const QString& label);

    void setCovType(ptn::modules::reachability::CovType);
    ptn::modules::reachability::CovType covType() const;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr)
        override;

    void updateLayout();

protected:
    void onLabelChanged();
    void onDrawNodeChanged();

private:
    QRectF m_boundingRect;
    QPainterPath m_path;
    QString m_label;
    QFont m_labelFont;
    QPointF m_labelPos;

    Node* m_node;

    ptn::modules::reachability::CovType m_type = ptn::modules::reachability::CovType::Inner;
};

#endif // GPNE_REACHABILITY_NODE_H
