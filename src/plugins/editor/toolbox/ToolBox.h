// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_TOOLBOX_H
#define GPNE_TOOLBOX_H

#include <QFrame>

class Tool;

class ToolBox : public QFrame {

    Q_OBJECT

public:
    enum ToolArea
    {
        TopLeft = 0,
        TopRight,
        BottomLeft,
        BottomRight
    };

    explicit ToolBox(QWidget* parent = nullptr);

    void setButtonSize(const QSize& size);

    void setToolArea(ToolArea area);

    void addTool(QAction* tool);
    void addTool(QMenu* tool);

    QPointF areaPoint() const;

    bool eventFilter(QObject *watched, QEvent *event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    QList<Tool*> m_buttons;
    QSize m_buttonSize;
    ToolArea m_toolArea = TopLeft;
};

#endif // GPNE_TOOLBOX_H
