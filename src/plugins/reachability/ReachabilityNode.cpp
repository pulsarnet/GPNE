// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityNode.h"
#include <corelib/viz/Graph.h>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QToolTip>

#define PEN_WIDTH 1
#define WIDTH 30
#define HEIGHT 30
#define SPACING_VERTICAL HEIGHT * 3.
#define SPACING_HORIZONTAL WIDTH * 1.5

/**
 * Creates an HTML table with headers and values.
 *
 * @param values List of integer values.
 * @param headers List of vertex indices.
 * @return QString containing the HTML table.
 */
QString create_text_with_headers(
    const QList<int32_t>& values,
    const QList<ptn::net::vertex::VertexIndex>& headers
)
{
    Q_ASSERT(values.size() == headers.size());

    QString thead, tbody;
    for (int i = 0; i < values.size(); ++i) {
        thead += QString("<th>P%1</th>").arg(headers[i].id);
        tbody += QString("<td align='center'>%1</td>")
                     .arg(values[i] < 0 ? QString("w") : QString::number(values[i]));
    }

    return QString("<table><thead><tr>%1</tr></thead><tbody><tr>%2</tr></tbody></table>")
        .arg(thead)
        .arg(tbody);
}

/**
 * Creates a comma-separated string with headers and values.
 *
 * @param values List of integer values.
 * @param headers List of vertex indices.
 * @return QString containing the formatted string.
 */
QString create_body_text_with_headers(
    const QList<int32_t>& values,
    const QList<ptn::net::vertex::VertexIndex>& headers
)
{
    Q_ASSERT(values.size() == headers.size());

    QString result;
    for (int i = 0; i < values.size(); ++i) {
        if (values[i] < 0) {
            result += QString("P%1(w),").arg(headers[i].id);
        } else if (values[i] > 0) {
            if (values[i] == 1) {
                result += QString("P%1,").arg(headers[i].id);
            } else {
                result += QString("P%1(%2),").arg(headers[i].id).arg(values[i]);
            }
        }
    }

    if (result.isEmpty()) {
        result = "empty";
    } else {
        result.chop(1); // Remove the last comma
    }
    return result;
}

/**
 * Constructor for ReachabilityNode.
 *
 * @param values List of integer values.
 * @param headers List of vertex indices.
 * @param parent Parent QGraphicsItem.
 */
ReachabilityNode::ReachabilityNode(
    const QList<int32_t>& values,
    const QList<ptn::net::vertex::VertexIndex>& headers,
    QGraphicsItem* parent
)
    : QGraphicsItem(parent)
    , m_node(nullptr)
    , m_labelFont(QFont())
{
    setToolTip(create_text_with_headers(values, headers));
    m_label = create_body_text_with_headers(values, headers);
    onLabelChanged();
    setAcceptHoverEvents(true);
}

/**
 * Returns the associated Node object.
 *
 * @return Pointer to the Node object.
 */
Node* ReachabilityNode::drawNode() const { return m_node; }

/**
 * Sets the associated Node object.
 *
 * @param node Pointer to the Node object.
 */
void ReachabilityNode::setDrawNode(Node* node)
{
    if (m_node == node) {
        return;
    }

    m_node = node;
    onDrawNodeChanged();
}

/**
 * Sets the label text.
 *
 * @param label The label text.
 */
void ReachabilityNode::setLabel(const QString& label)
{
    if (m_label == label) {
        return;
    }


    m_label = label;
    onLabelChanged();
}

/**
 * Sets the coverage type.
 *
 * @param type The coverage type.
 */
void ReachabilityNode::setCovType(ptn::modules::reachability::CovType type) { m_type = type; }

/**
 * Returns the coverage type.
 *
 * @return The coverage type.
 */
ptn::modules::reachability::CovType ReachabilityNode::covType() const { return m_type; }

/**
 * Returns the bounding rectangle of the item.
 *
 * @return QRectF representing the bounding rectangle.
 */
QRectF ReachabilityNode::boundingRect() const { return m_boundingRect; }

/**
 * Paints the item.
 *
 * @param painter The QPainter object.
 * @param option The style options for the item.
 * @param widget The widget being painted on.
 */
void ReachabilityNode::paint(
QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget
)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)
    if (!m_node) {
        return;
    }

    QColor penColor;
    if (isSelected()) {
        penColor = QColor(Qt::green);
    } else {
        auto type = covType();
        switch (type) {
            case ptn::modules::reachability::CovType::Duplicate:
                penColor = Qt::blue;
            break;
            case ptn::modules::reachability::CovType::DeadEnd:
                penColor = Qt::red;
            break;
            case ptn::modules::reachability::CovType::Inner:
            case ptn::modules::reachability::CovType::Boundary:
            default:
                penColor = Qt::black;
        }
    }


    painter->setRenderHints(
        QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform
    );
    painter->setPen(QPen(penColor, PEN_WIDTH));
    painter->setFont(m_labelFont);
    painter->drawPath(m_path);
    painter->drawText(m_labelPos, m_label);
}

/**
 * Updates the layout of the item.
 */
void ReachabilityNode::updateLayout()
{
    if (!m_node) {
        return;
    }

    setPos(m_node->x(), m_node->y() * SPACING_VERTICAL);

    update();
}

/**
 * Updates the label position and bounding rectangle.
 */
void ReachabilityNode::onLabelChanged()
{
    const QFontMetricsF fm(m_labelFont);
    QRectF textRect = fm.boundingRect(m_label);

    m_boundingRect = QRectF(-textRect.width() / 2., -HEIGHT / 2., textRect.width(), HEIGHT)
                         .adjusted(-5, 0, 5, 0);

    m_path = QPainterPath();
    m_path.addRect(m_boundingRect);

    qreal x = m_boundingRect.x() + (m_boundingRect.width() - textRect.width()) / 2.;
    qreal y = m_boundingRect.y() + (m_boundingRect.height() - textRect.height()) / 2. + fm.ascent();

    m_labelPos = QPointF(x, y);
}

/**
 * Updates the associated Node object when it changes.
 */
void ReachabilityNode::onDrawNodeChanged()
{
    if (m_node) {
        m_node->setWidth(m_boundingRect.width());
    }
}
