// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "DoubleRangeWidget.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

DoubleRangeWidget::DoubleRangeWidget(QWidget* parent)
    : QWidget(parent)
    , m_beginSpinBox(new QDoubleSpinBox(this))
    , m_endSpinBox(new QDoubleSpinBox(this))
    , m_resetButton(new QPushButton(this))
{
    m_beginSpinBox->setRange(0, 0);
    m_beginSpinBox->setValue(0);
    m_beginSpinBox->setSingleStep(1.0);
    m_beginSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_beginSpinBox->setFixedWidth(60);

    m_endSpinBox->setRange(0, 0);
    m_endSpinBox->setValue(0);
    m_endSpinBox->setSingleStep(1.0);
    m_endSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_endSpinBox->setFixedWidth(60);

    m_resetButton->setFlat(true);
    m_resetButton->setIcon(QIcon(":/images/icons/reset.svg"));

    connect(
        m_beginSpinBox,
        &QDoubleSpinBox::valueChanged,
        this,
        &DoubleRangeWidget::onBeginValueChanged
    );

    connect(
        m_endSpinBox,
        &QDoubleSpinBox::valueChanged,
        this,
        &DoubleRangeWidget::onEndValueChanged
    );

    connect(
        m_resetButton,
        &QPushButton::clicked,
        this,
        &DoubleRangeWidget::reset
    );

    auto mainLayout = new QHBoxLayout(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_beginSpinBox);
    mainLayout->addWidget(new QLabel(tr(" to ")));
    mainLayout->addWidget(m_endSpinBox);
    mainLayout->addWidget(m_resetButton);
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    setLayout(mainLayout);
}

void DoubleRangeWidget::setRange(double min, double max)
{
    if (min <= max) {
        m_beginSpinBox->blockSignals(true);
        m_endSpinBox->blockSignals(true);

        m_beginSpinBox->setMinimum(min);
        m_endSpinBox->setMaximum(max);

        m_beginSpinBox->blockSignals(false);
        m_endSpinBox->blockSignals(false);
    }
}

void DoubleRangeWidget::setDecimal(int decimal)
{
    m_beginSpinBox->setDecimals(decimal);
    m_endSpinBox->setDecimals(decimal);
}

void DoubleRangeWidget::setBegin(std::optional<double> begin)
{
    if (begin.has_value()) {
        m_beginSpinBox->setValue(*begin);
    } else {
        m_beginSpinBox->setValue(m_beginSpinBox->minimum());
    }
}

std::optional<double> DoubleRangeWidget::begin() const noexcept
{
    double value = m_beginSpinBox->value();
    return value == m_beginSpinBox->minimum() ? std::nullopt : std::make_optional(value);
}

void DoubleRangeWidget::setEnd(std::optional<double> end)
{
    if (end.has_value()) {
        m_endSpinBox->setValue(*end);
    } else {
        m_endSpinBox->setValue(m_endSpinBox->maximum());
    }
}

std::optional<double> DoubleRangeWidget::end() const noexcept
{
    double value = m_endSpinBox->value();
    return value == m_endSpinBox->maximum() ? std::nullopt : std::make_optional(value);
}

void DoubleRangeWidget::onBeginValueChanged(double)
{
    updateState();
}

void DoubleRangeWidget::onEndValueChanged(double)
{
    updateState();
}

void DoubleRangeWidget::reset() {
    m_beginSpinBox->setValue(m_beginSpinBox->minimum());
    m_endSpinBox->setValue(m_endSpinBox->maximum());
}

void DoubleRangeWidget::updateState()
{
    m_beginSpinBox->setMaximum(m_endSpinBox->value());
    m_endSpinBox->setMinimum(m_beginSpinBox->value());
}
