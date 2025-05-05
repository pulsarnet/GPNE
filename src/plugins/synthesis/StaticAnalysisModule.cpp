// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "StaticAnalysisModule.h"

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <ptn/decompose.h>
#include <ptn/net.h>
#include <ranges>

#include "DoubleRangeWidget.h"

class QHBoxLayout;

StaticAnalysisModule::StaticAnalysisModule()
    : AbstractAnalysisModule()
    , m_validatePlaces(false)
    , m_validateTransitions(false)
    , m_maxEdgeWeight(0)
{
}

void StaticAnalysisModule::saveState(QSettings &) const {
    // settings.beginGroup("places");
    // settings.setValue("min", m_places.start.has_value() ? *m_places.start : QVariant());
    // settings.setValue("max", m_places.end.has_value() ? *m_places.end : QVariant());
    // settings.endGroup();
    //
    // settings.beginGroup("transitions");
    // settings.setValue("min", m_transitions.start.has_value() ? *m_transitions.start : QVariant());
    // settings.setValue("max", m_transitions.end.has_value() ? *m_transitions.end : QVariant());
    // settings.endGroup();
    //
    // settings.setValue("maxEdgeWeight", m_maxEdgeWeight);
}

void StaticAnalysisModule::loadState(const QSettings &) {

}

// todo replace AbstractSettingsEditor as QWidget
QWidget *StaticAnalysisModule::createSettingsEditor(context_type *ctx) const {
    auto settingsEditor = new StaticSettingsEditor();
    settingsEditor->setPlaceBounds(1, ctx->lbf()->places_count());
    settingsEditor->setTransitionBounds(1, ctx->lbf()->transitions_count());
    return settingsEditor;
}

void StaticAnalysisModule::restoreEditorData(QWidget *editor) {
    auto settingsEditor = qobject_cast<StaticSettingsEditor *>(editor);
    if (settingsEditor) {
        settingsEditor->setPlaceRange(m_places);
        settingsEditor->setTransitionRange(m_transitions);
        settingsEditor->setMaxEdgeWeight(m_maxEdgeWeight);
    }
}

void StaticAnalysisModule::applyEditorData(QWidget *editor) {
    auto settingsEditor = qobject_cast<StaticSettingsEditor *>(editor);
    if (settingsEditor) {
        setPlacesRange(settingsEditor->placeRange());
        setTransitionsRange(settingsEditor->transitionRange());
        setMaxEdgeWeight(settingsEditor->maxEdgeWeight());
    }
}

bool StaticAnalysisModule::canDisable() const noexcept {
    return false;
}

bool StaticAnalysisModule::isNeedValidation() const noexcept {
    return m_validatePlaces || m_validateTransitions || m_maxEdgeWeight > 0;
}

bool StaticAnalysisModule::run(ptn::net::PetriNet *net, std::span<QVariant> metrics, double& metricCost) const {
    int places = net->places_count();
    int transitions = net->transitions_count();

    metrics[0] = places;
    metrics[1] = transitions;

    auto directed = net->directed_arcs();
    auto it = std::ranges::max_element(directed, [](auto& a, auto& b) { return a.weight() < b.weight(); });
    int maxWeight = 0;
    if (it != directed.end()) {
        maxWeight = it->weight();
    }

    metricCost += m_places.diff(places) * 100;
    metricCost += m_transitions.diff(transitions) * 100;
    metricCost += m_maxEdgeWeight == 0 ? 0 : std::max(0, maxWeight - m_maxEdgeWeight);

    if (m_validatePlaces && !m_places.isMatch(places)) {
        return false;
    }

    if (m_validateTransitions && !m_transitions.isMatch(transitions)) {
        return false;
    }

    if (m_maxEdgeWeight && m_maxEdgeWeight < maxWeight) {
        return false;
    }

    return true;
}

QString StaticAnalysisModule::displayName() const noexcept {
    return QObject::tr("Static analisys");
}

int StaticAnalysisModule::metricCount() const {
    return 2;
}

QString StaticAnalysisModule::metricName(int idx) const {
    switch (idx) {
        case 0:
            return "Places";
        case 1:
            return "Transitions";
        default:
            return "";
    }
}

QMetaType::Type StaticAnalysisModule::metricType(int idx) const {
    switch (idx) {
        case 0:
        case 1:
            return QMetaType::Int;
        default:
            return QMetaType::UnknownType;
    }
}

Range StaticAnalysisModule::placesRange() const noexcept { return m_places; }

void StaticAnalysisModule::setPlacesRange(Range range) {
    m_places = range;
    m_validatePlaces = !m_places.isFullUnbound();
}

Range StaticAnalysisModule::transitionsRange() const noexcept {
    return m_transitions;
}

void StaticAnalysisModule::setTransitionsRange(Range range) {
    m_transitions = range;
    m_validateTransitions = !m_transitions.isFullUnbound();
}

void StaticAnalysisModule::setMaxEdgeWeight(int range) {
    m_maxEdgeWeight = range;
}

int StaticAnalysisModule::maxEdgeWeight() const noexcept {
    return m_maxEdgeWeight;
}

StaticSettingsEditor::StaticSettingsEditor(QWidget* parent)
    : QWidget(parent)
    , m_placesRange(new DoubleRangeWidget(this))
    , m_transitionsRange(new DoubleRangeWidget(this))
    , m_maxEdgeWeight(new QSpinBox)
{
    m_placesRange->setRange(1, 1);
    m_transitionsRange->setRange(1, 1);
    m_maxEdgeWeight->setRange(1, SHRT_MAX);

    m_placesRange->setDecimal(0);
    m_transitionsRange->setDecimal(0);

    m_maxEdgeWeight->setRange(0, USHRT_MAX);
    m_maxEdgeWeight->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_maxEdgeWeight->setFixedWidth(60);

    auto maxEdgeHelpLabel = new QLabel(this);
    maxEdgeHelpLabel->setText("(0 = all)");
    maxEdgeHelpLabel->setStyleSheet("color: gray;");

    QHBoxLayout* maxEdgeStoreLayout = new QHBoxLayout;
    maxEdgeStoreLayout->setContentsMargins(0, 0, 0, 0);
    maxEdgeStoreLayout->addWidget(m_maxEdgeWeight);
    maxEdgeStoreLayout->addWidget(maxEdgeHelpLabel);

    auto* mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(new QLabel(tr("Number of places"), this), 0, 0);
    mainLayout->addWidget(m_placesRange, 0, 1);

    mainLayout->addWidget(new QLabel(tr("Number of transitions"), this), 1, 0);
    mainLayout->addWidget(m_transitionsRange, 1, 1);

    mainLayout->addWidget(new QLabel(tr("Max edge weight"), this), 2, 0);
    mainLayout->addLayout(maxEdgeStoreLayout, 2, 1);

    mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 0, 1, 2);

    setLayout(mainLayout);
}

void StaticSettingsEditor::setPlaceBounds(double min, double max) {
    m_placesRange->setRange(min, max);
}

void StaticSettingsEditor::setPlaceRange(Range range) {
    m_placesRange->setBegin(range.start);
    m_placesRange->setEnd(range.end);
}

Range StaticSettingsEditor::placeRange() const {
    return Range(m_placesRange->begin(), m_placesRange->end());
}

void StaticSettingsEditor::setTransitionBounds(double min, double max) {
    m_transitionsRange->setRange(min, max);
}

void StaticSettingsEditor::setTransitionRange(Range range) {
    m_transitionsRange->setBegin(range.start);
    m_transitionsRange->setEnd(range.end);
}

Range StaticSettingsEditor::transitionRange() const {
    return Range(m_transitionsRange->begin(), m_transitionsRange->end());
}

void StaticSettingsEditor::setMaxEdgeWeight(int maxWeight) {
    m_maxEdgeWeight->setValue(maxWeight);
}

int StaticSettingsEditor::maxEdgeWeight() const {
    return m_maxEdgeWeight->value();
}
