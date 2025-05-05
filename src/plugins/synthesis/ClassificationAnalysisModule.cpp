// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ClassificationAnalysisModule.h"
#include "AbstractAnalysisModule.h"
#include <QListWidget>
#include <QGridLayout>
#include <QLabel>

ClassificationAnalysisModule::ClassificationAnalysisModule()
    : AbstractAnalysisModule()
{
}

QWidget* ClassificationAnalysisModule::createSettingsEditor(context_type *) const {
    return new ClassificationSettingsEditor();
}

void ClassificationAnalysisModule::restoreEditorData(QWidget *editor) {
    // TODO:
}

void ClassificationAnalysisModule::applyEditorData(QWidget *editor) {
    // TODO
}

void ClassificationAnalysisModule::saveState(QSettings &settings) const {
}

void ClassificationAnalysisModule::loadState(const QSettings &settings) {
}

bool ClassificationAnalysisModule::run(ptn::net::PetriNet *net, std::span<QVariant> metrics, double &metricCost) const {
    metricCost = 0.;
    metrics[0] = "First class";
    return true;
}

QString ClassificationAnalysisModule::displayName() const noexcept {
    return QObject::tr("Classification analysis");
}

int ClassificationAnalysisModule::metricCount() const {
    return 1;
}

QString ClassificationAnalysisModule::metricName(int idx) const {
    return "Petri net class";
}

QMetaType::Type ClassificationAnalysisModule::metricType(int idx) const {
    return QMetaType::QString;
}

bool ClassificationAnalysisModule::isNeedValidation() const noexcept {
    return true;
}

ClassificationSettingsEditor::ClassificationSettingsEditor(QWidget *parent)
    : QWidget(parent)
    , m_netClass(new QListWidget(this))
{
    m_netClass->setMinimumHeight(80);
    m_netClass->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QStringList netClassList = {"Free choice", "Marked graph", "State Machine", "Conflict Free"};
    for (const auto& netClass : netClassList) {
        QListWidgetItem *item = new QListWidgetItem(netClass);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        m_netClass->addItem(item);
    }

    auto* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Petri net classes:"), this), 0, 2);
    mainLayout->addWidget(m_netClass, 1, 2, 3, 1);

    setLayout(mainLayout);
}
