// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisConfigurator.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QThread>
#include <QVBoxLayout>

SynthesisConfigurator::SynthesisConfigurator(QWidget* parent) : QDialog(parent)
{
    QWidget* settingsContainer = new QWidget(this);
    m_settingsContainerLayout = new QVBoxLayout(settingsContainer);
    m_settingsContainerLayout->setContentsMargins(0, 0, 0, 0);
    m_settingsContainerLayout->setSpacing(0);
    m_settingsContainerLayout->addStretch(1);

    // add setting for thread count
    // add setting for generator configuration
    auto generalSettingsHLayout = new QHBoxLayout();
    auto generalSettingsGridLayout = new QGridLayout();
    generalSettingsHLayout->addLayout(generalSettingsGridLayout);
    generalSettingsHLayout->addStretch(1);
    auto generalSettingsGroup = new QGroupBox(tr("General settings"), this);
    generalSettingsGroup->setFlat(true);
    generalSettingsGroup->setLayout(generalSettingsHLayout);

    auto threadCount = new QSpinBox(this);
    threadCount->setRange(1, QThread::idealThreadCount());
    generalSettingsGridLayout->addWidget(new QLabel(tr("Thread count"), this), 0, 0);
    generalSettingsGridLayout->addWidget(threadCount, 0, 1);

    auto limitTotalNumberEvaluated = new QSpinBox(this);
    limitTotalNumberEvaluated->setRange(1, 200'000'000);
    limitTotalNumberEvaluated->setValue(limitTotalNumberEvaluated->maximum());
    generalSettingsGridLayout->addWidget(new QLabel(tr("Limit total number evaluated models"), this), 1, 0);
    generalSettingsGridLayout->addWidget(limitTotalNumberEvaluated, 1, 1);

    auto requiredNumberStructures = new QSpinBox(this);
    requiredNumberStructures->setRange(1, 100);
    requiredNumberStructures->setValue(requiredNumberStructures->maximum());
    generalSettingsGridLayout->addWidget(new QLabel(tr("Maximum solutions"), this), 2, 0);
    generalSettingsGridLayout->addWidget(requiredNumberStructures, 2, 1);

    auto timeLimit = new QSpinBox(this);
    timeLimit->setRange(1, INT_MAX);
    timeLimit->setValue(timeLimit->maximum());
    generalSettingsGridLayout->addWidget(new QLabel(tr("Time limit (sec)"), this), 3, 0);
    generalSettingsGridLayout->addWidget(timeLimit, 3, 1);

    auto generatorLimitation = new QComboBox(this);
    generatorLimitation->addItem(QIcon(), tr("No rules"), 0);
    generatorLimitation->addItem(QIcon(), tr("Linear base fragment integrity"), 1);
    generalSettingsGridLayout->addWidget(new QLabel(tr("Knowledge base rules"), this), 4, 0);
    generalSettingsGridLayout->addWidget(generatorLimitation, 4, 1);

    m_settingsContainerLayout->addWidget(generalSettingsGroup);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(settingsContainer);

    auto l = new QVBoxLayout();
    l->addWidget(m_scrollArea);
    setLayout(l);
    setupButtons();

    setMinimumWidth(600);
    setMinimumHeight(400);
}

void SynthesisConfigurator::addEditor(QWidget *editor, const QString& name, bool canDisable) noexcept {
    QGroupBox* group = new QGroupBox(name, this);
    group->setCheckable(canDisable);
    group->setFlat(true);
    group->setChecked(true);
    group->setLayout(new QVBoxLayout);
    group->layout()->addWidget(editor);

    m_settingsContainerLayout->addWidget(group);
}

void SynthesisConfigurator::setupButtons()
{
    QDialogButtonBox* buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Run");
    buttons->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/simulation/run.svg"));

    connect(buttons, &QDialogButtonBox::accepted, this, &SynthesisConfigurator::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SynthesisConfigurator::reject);
    layout()->addWidget(buttons);
}
