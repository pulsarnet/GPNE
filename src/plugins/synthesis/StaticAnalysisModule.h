// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef STATICANALYSISMODULE_H
#define STATICANALYSISMODULE_H

#include "AbstractAnalysisModule.h"
#include "Range.h"
#include <QWidget>

class DoubleRangeWidget;
class QSpinBox;

class StaticAnalysisModule final : public AbstractAnalysisModule  {
public:
    StaticAnalysisModule();
    virtual ~StaticAnalysisModule() = default;

    void saveState(QSettings &settings) const override;
    void loadState(const QSettings &settings) override;

    QWidget* createSettingsEditor(context_type*) const override;

    void restoreEditorData(QWidget *editor) override;
    void applyEditorData(QWidget *editor) override;

    bool canDisable() const noexcept override;

    bool run(ptn::net::PetriNet*, std::span<QVariant>, double& metricCost) const override;

    QString displayName() const noexcept override;

    int metricCount() const override;
    QString metricName(int) const override;
    QMetaType::Type metricType(int) const override;

    Range placesRange() const noexcept;
    void setPlacesRange(Range range);

    Range transitionsRange() const noexcept;
    void setTransitionsRange(Range range);

    int maxEdgeWeight() const noexcept;
    void setMaxEdgeWeight(int range);

protected:

    bool isNeedValidation() const noexcept override;

private:
    Range m_places;
    Range m_transitions;
    int m_maxEdgeWeight;

    bool m_validatePlaces;
    bool m_validateTransitions;
};

class StaticSettingsEditor : public QWidget {
    Q_OBJECT
public:
    StaticSettingsEditor(QWidget* parent = nullptr);
    virtual ~StaticSettingsEditor() = default;

    void setPlaceBounds(double min, double max);
    void setPlaceRange(Range range);
    Range placeRange() const;

    void setTransitionBounds(double min, double max);
    void setTransitionRange(Range range);
    Range transitionRange() const;

    void setMaxEdgeWeight(int maxWeight);
    int maxEdgeWeight() const;

private:
    DoubleRangeWidget* m_placesRange;
    DoubleRangeWidget* m_transitionsRange;
    QSpinBox* m_maxEdgeWeight;
};

#endif //STATICANALYSISMODULE_H
