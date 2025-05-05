// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ToolBox.h"
#include "Tool.h"
#include <QVBoxLayout>
#include <QResizeEvent>

ToolBox::ToolBox(QWidget* parent) : QFrame()
{
    parent->installEventFilter(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_MacAlwaysShowToolWindow, true);
    setAttribute(Qt::WA_Resized, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    this->setLayout(new QVBoxLayout);
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 3, 0, 3);
    this->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    setParent(parent);
}

void ToolBox::setButtonSize(const QSize& size) { m_buttonSize = size; }

void ToolBox::setToolArea(ToolArea area) { m_toolArea = area; }

void ToolBox::addTool(QAction* tool)
{
    auto button = new Tool(tool);
    dynamic_cast<QVBoxLayout*>(this->layout())->addWidget(button);
    button->setSize(m_buttonSize);
    m_buttons.push_back(button);

    resizeEvent(nullptr);
}

void ToolBox::addTool(QMenu* menu)
{
    auto button = new Tool(menu);
    dynamic_cast<QVBoxLayout*>(this->layout())->addWidget(button);
    button->setSize(m_buttonSize);
    m_buttons.push_back(button);

    resizeEvent(nullptr);
}

QPointF ToolBox::areaPoint() const
{
    switch (m_toolArea) {
    case TopLeft:
        return QPointF(
            (parentWidget() ? parentWidget()->geometry().topLeft().x() : 0) + 20,
            (parentWidget() ? parentWidget()->geometry().topLeft().y() : 0) + 20
        );
    case TopRight:
        return QPointF(
            (parentWidget() ? parentWidget()->geometry().topRight().x() : 0) - 20 -
                m_buttonSize.width(),
            (parentWidget() ? parentWidget()->geometry().topRight().y() : 0) + 20
        );
    case BottomLeft:
        return QPointF(
            (parentWidget() ? parentWidget()->geometry().bottomLeft().x() : 0) + 20,
            (parentWidget() ? parentWidget()->geometry().bottomLeft().y() : 0) - 20 -
                qreal((m_buttonSize.height() + 6) * m_buttons.count())
        );
    case BottomRight:
    default:
        return QPointF(
            (parentWidget() ? parentWidget()->geometry().bottomRight().x() : 0) - 20 -
                m_buttonSize.width(),
            (parentWidget() ? parentWidget()->geometry().bottomRight().y() : 0) - 20 -
                qreal((m_buttonSize.height() + 6) * m_buttons.count())
        );
    }
}

bool ToolBox::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::Resize && watched != this) {
        resizeEvent(static_cast<QResizeEvent*>(event));
    }
    return QFrame::eventFilter(watched, event);
}

void ToolBox::resizeEvent(QResizeEvent* event)
{
    {
        auto point = areaPoint();
        this->setGeometry(
            point.x(),
            point.y(),
            m_buttonSize.width(),
            (m_buttonSize.height() + 6) * m_buttons.count()
        );

        QWidget::resizeEvent(event);
    }
}
