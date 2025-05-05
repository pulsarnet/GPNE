// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SimulationAnalysisModule.h"
#include "DoubleRangeWidget.h"
#include <ptn/simulation.h>
#include <QGridLayout>
#include <ptn/net.h>
#include <QCheckBox>
#include <QLabel>

SimulationAnalysisModule::SimulationAnalysisModule()
    : AbstractAnalysisModule()
    , m_allowDeadTransitions(false)
    , m_validateUtilization(false)
{
}

QWidget *SimulationAnalysisModule::createSettingsEditor(context_type *context) const {
    return new SimulationSettingsEditor();
}

void SimulationAnalysisModule::restoreEditorData(QWidget *editor) {
    auto settingsEditor = qobject_cast<SimulationSettingsEditor *>(editor);
    if (settingsEditor) {
        settingsEditor->setIterationRange(m_iterations);
        settingsEditor->setUtilizationRange(m_utilization);
        settingsEditor->setAllowDeadTransitions(m_allowDeadTransitions);
    }
}

void SimulationAnalysisModule::applyEditorData(QWidget *editor) {
    auto settingsEditor = qobject_cast<SimulationSettingsEditor *>(editor);
    if (settingsEditor) {
        setIterationsRange(settingsEditor->iterationRange());
        setUtilizationRange(settingsEditor->utilizationRange());
        setAllowDeadTransitions(settingsEditor->allowDeadTransitions());
    }
}

bool SimulationAnalysisModule::isNeedValidation() const noexcept {
    return !m_allowDeadTransitions || m_validateUtilization;
}

bool SimulationAnalysisModule::run(ptn::net::PetriNet *net, std::span<QVariant> metrics, double& metricCost) const {
    auto simulation = ptn::modules::simulation::Simulation::init(net);
    size_t limit = m_iterationsLimit;
    int iterations = 0;
    double utilization = 0.0;

    std::vector<usize> notFiredTransitions;
    if (!m_allowDeadTransitions) {
        auto transitions = net->transitions();
        for (auto transition: transitions) {
            notFiredTransitions.push_back(transition);
        }
    }

    while (limit > 0) {
        int fired = simulation->step();
        if (fired == 0) {
            break;
        }

        limit--;
        iterations++;
        utilization += fired;

        if (!m_allowDeadTransitions) {
            const auto [first, last] = std::ranges::remove_if(notFiredTransitions, [&](const usize &transition) {
                return simulation->is_fired({vertex::VertexType::TTransition, transition});
            });
            notFiredTransitions.erase(first, last);
        }
    }

    if (iterations > 0) {
        utilization /= iterations * net->transitions_count();
        utilization *= 100;
    }

    metrics[0] = utilization;
    metrics[1] = limit == 0 ? -1 : iterations;
    simulation->drop();

    metricCost += m_iterations.diff(iterations);
    metricCost += m_utilization.diff(utilization);
    metricCost += (!m_allowDeadTransitions && !notFiredTransitions.empty());

    if (!m_iterations.isMatch(iterations)) {
        return false;
    }

    if (m_validateUtilization && !m_utilization.isMatch(utilization)) {
        return false;
    }

    if (!m_allowDeadTransitions) {
        if (!notFiredTransitions.empty()) {
            return false;
        }
    }

    return true;
}

void SimulationAnalysisModule::saveState(QSettings &settings) const {
}

void SimulationAnalysisModule::loadState(const QSettings &settings) {
}

QString SimulationAnalysisModule::displayName() const noexcept {
    return "Behavior analisys";
}

int SimulationAnalysisModule::metricCount() const {
    return 2;
}

QString SimulationAnalysisModule::metricName(int idx) const {
    switch (idx) {
        case 0:
            return "Utilization";
        case 1:
            return "Iterations";
        default:
            return "";
    }
}

QMetaType::Type SimulationAnalysisModule::metricType(int idx) const {
    switch (idx) {
        case 0:
            return QMetaType::Double;
        case 1:
            return QMetaType::Int;
        default:
            return QMetaType::UnknownType;
    }
}

Range SimulationAnalysisModule::iterationsRange() const noexcept {
    return m_iterations;
}

void SimulationAnalysisModule::setIterationsRange(Range range) {
    m_iterations = range;
    m_iterationsLimit = range.end.value_or(11) + 2;
}

Range SimulationAnalysisModule::utilizationRange() const noexcept {
    return m_utilization;
}

void SimulationAnalysisModule::setUtilizationRange(Range range) {
    m_utilization = range;
    m_validateUtilization = !m_utilization.isFullUnbound();
}

bool SimulationAnalysisModule::allowDeadTransitions() const noexcept {
    return m_allowDeadTransitions;
}

void SimulationAnalysisModule::setAllowDeadTransitions(bool allow) {
    m_allowDeadTransitions = allow;
}

SimulationSettingsEditor::SimulationSettingsEditor(QWidget* parent)
    : QWidget(parent)
    , m_iterationsRange(new DoubleRangeWidget(this))
    , m_utilizationRange(new DoubleRangeWidget(this))
    , m_allowDeadTransitions(new QCheckBox(tr("Allow dead transitions"), this))
{
    m_iterationsRange->setRange(0, 10);
    m_iterationsRange->setDecimal(0);

    m_utilizationRange->setRange(0, 100);
    m_utilizationRange->setDecimal(2);

    m_allowDeadTransitions->setChecked(false);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_allowDeadTransitions, 0, 0);

    mainLayout->addWidget(new QLabel(tr("Number of firing steps"), this), 2, 0);
    mainLayout->addWidget(m_iterationsRange, 2, 1);

    mainLayout->addWidget(new QLabel(tr("Average utilization of transitions"), this), 3, 0);
    mainLayout->addWidget(m_utilizationRange, 3, 1);

    setLayout(mainLayout);
}

void SimulationSettingsEditor::setIterationRange(Range range) {
    m_iterationsRange->setBegin(range.start);
    m_iterationsRange->setEnd(range.end);
}

Range SimulationSettingsEditor::iterationRange() const {
    return Range(m_iterationsRange->begin(), m_iterationsRange->end());
}

void SimulationSettingsEditor::setUtilizationRange(Range range) {
    m_utilizationRange->setBegin(range.start);
    m_utilizationRange->setEnd(range.end);
}

Range SimulationSettingsEditor::utilizationRange() const {
    return Range(m_utilizationRange->begin(), m_utilizationRange->end());
}

void SimulationSettingsEditor::setAllowDeadTransitions(bool allowDeadTransitions) {
    m_allowDeadTransitions->setChecked(allowDeadTransitions);
}

bool SimulationSettingsEditor::allowDeadTransitions() const {
    return m_allowDeadTransitions;
}
