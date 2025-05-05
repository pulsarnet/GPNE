// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "Tool.h"

#include <QEvent>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QMenu>
#include <QPainterPath>
#include <QToolTip>

Tool::Tool(QAction* action, QWidget* parent) : QToolButton(parent)
{
    this->setDefaultAction(action);
    this->setMouseTracking(true);
    this->setCheckable(true);
    this->setAttribute(Qt::WA_Hover);

    connect(
        this,
        &QToolButton::triggered,
        this,
        &Tool::onTriggered
    ); // NOLINT(*-unused-return-value)
}

Tool::Tool(QMenu* menu, QWidget* parent) : QToolButton(parent)
{
    if (!menu->actions().isEmpty()) {
        this->setDefaultAction(menu->actions().first());
    }

    this->setMenu(menu);
    this->setMouseTracking(true);
    this->setCheckable(true);
    this->setAttribute(Qt::WA_Hover);

    menu->setObjectName("tool-bar-menu");
    menu->installEventFilter(this);

    // When an action is triggered by a shortcut before selecting the action in
    // menu, the QToolButton::triggered signal is not emitted. This happens
    // because the QToolButton::setMenu does not set a parent for the menu.
    connect(menu, &QMenu::triggered, this, &Tool::onTriggered); // NOLINT(*-unused-return-value)
}

void Tool::setSize(const QSize& size)
{
    this->setFixedSize(size);
    this->setIconSize(size);
}

void Tool::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QStyleOptionToolButton opts;
    initStyleOption(&opts);

    QColor highlightedBack(0, 166, 251);
    QColor back(94, 94, 94); // E6E6E9
    QColor hover(135, 134, 134);

    if (opts.state & QStyle::State_On) {
        painter.setBackground(QBrush(highlightedBack));
    } else if (opts.state & QStyle::StateFlag::State_MouseOver) {
        painter.setBackground(QBrush(hover));
    } else {
        painter.setBackground(QBrush(back));
    }

    painter.setOpacity(0.7);

    QRect rect_(rect());
    QPainterPath path;
    path.addRoundedRect(rect_, 5, 5);
    painter.fillPath(path, painter.background());

    painter.setOpacity(1.);

    QRect iconRect = rect().adjusted(5, 5, -5, -5);
    opts.icon.paint(&painter, iconRect);
    // if (defaultAction()) defaultAction()->icon().paint(&painter, iconRect);

    if (opts.features & QStyleOptionToolButton::HasMenu) {
        QPainterPath arrowPath;
        QPoint bottomRight(rect_.bottomRight());
        bottomRight.setX(bottomRight.x() - 1);
        bottomRight.setY(bottomRight.y() - 1);

        arrowPath.moveTo(bottomRight);
        arrowPath.lineTo(bottomRight.x(), bottomRight.y() - 6);
        arrowPath.lineTo(bottomRight.x() - 6, bottomRight.y());
        arrowPath.lineTo(bottomRight);

        painter.setBrush(Qt::black);
        painter.drawPath(arrowPath);
    }
}

void Tool::onTriggered(QAction* action)
{
    qDebug() << "Tool::onTriggered: called by" << sender();
    setDefaultAction(action);
}

bool Tool::event(QEvent* e)
{
    if (e->type() == QEvent::ToolTip) {
        auto tooltipPos =
            QPoint(this->pos().x() + this->geometry().width() + 5, this->pos().y() - 16);

        QString toolTipText = QString("<b>%1</b>").arg(defaultAction()->text());
        toolTipText.append(QString("<p>%1</p>").arg(toolTip()));
        if (QAction* action = this->defaultAction(); action && !action->shortcut().isEmpty()) {
            toolTipText.append(QString("<p><i>Shortcut: %1<i></p>")
                                   .arg(action->shortcut().toString(QKeySequence::NativeText)));
        }

        QToolTip::showText(
            this->parentWidget()->mapToGlobal(tooltipPos),
            toolTipText,
            this,
            QRect(),
            toolTipDuration()
        );
        return true;
    }

    return QToolButton::event(e);
}

void Tool::enterEvent(QEnterEvent*) { setCursor(Qt::PointingHandCursor); }

void Tool::leaveEvent(QEvent*) { setCursor(Qt::ArrowCursor); }

bool Tool::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Show && watched == menu()) {
        QPoint pos = rect().topRight();
        pos.setX(pos.x() + 3);
        menu()->move(mapToGlobal(pos));
        return true;
    }
    return QToolButton::eventFilter(watched, event);
}
