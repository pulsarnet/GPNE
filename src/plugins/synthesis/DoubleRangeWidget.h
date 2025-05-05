// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef DOUBLERANGEWIDGET_H
#define DOUBLERANGEWIDGET_H

#include <QWidget>

class QPushButton;
class QDoubleSpinBox;

class DoubleRangeWidget : public QWidget {
public:
    DoubleRangeWidget(QWidget* parent = nullptr);

    void setRange(double min, double max);

    void setDecimal(int decimal);

    void setBegin(std::optional<double> begin);
    std::optional<double> begin() const noexcept;

    void setEnd(std::optional<double> end);
    std::optional<double> end() const noexcept;

signals:

    void rangeChaged();

public slots:

    void onBeginValueChanged(double value);
    void onEndValueChanged(double value);
    void reset();

protected:

    void updateState();

private:

    QDoubleSpinBox* m_beginSpinBox;
    QDoubleSpinBox* m_endSpinBox;
    QPushButton* m_resetButton;
};



#endif //DOUBLERANGEWIDGET_H
