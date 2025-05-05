// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SIMULATIONANALYSISMODULE_H
#define SIMULATIONANALYSISMODULE_H

#include "AbstractAnalysisModule.h"
#include "Range.h"
#include <QWidget>

class QCheckBox;
class DoubleRangeWidget;

class SimulationAnalysisModule final : public AbstractAnalysisModule {
public:
    SimulationAnalysisModule();
    virtual ~SimulationAnalysisModule() = default;

    QWidget* createSettingsEditor(context_type*) const override;

    void restoreEditorData(QWidget *editor) override;
    void applyEditorData(QWidget *editor) override;

    bool run(ptn::net::PetriNet*, std::span<QVariant>, double& metricCost) const override;

    void saveState(QSettings &settings) const override;
    void loadState(const QSettings &settings) override;

    QString displayName() const noexcept override;

    int metricCount() const override;
    QString metricName(int) const override;
    QMetaType::Type metricType(int) const override;

    Range iterationsRange() const noexcept;
    void setIterationsRange(Range range);

    Range utilizationRange() const noexcept;
    void setUtilizationRange(Range range);

    bool allowDeadTransitions() const noexcept;
    void setAllowDeadTransitions(bool allow);

protected:

    bool isNeedValidation() const noexcept override;

private:

    int m_iterationsLimit = 10;
    Range m_iterations;
    Range m_utilization;
    bool m_allowDeadTransitions;

    bool m_validateUtilization;
};

class SimulationSettingsEditor : public QWidget {
    Q_OBJECT
public:
    SimulationSettingsEditor(QWidget* parent = nullptr);
    virtual ~SimulationSettingsEditor() = default;

    void setIterationRange(Range range);
    Range iterationRange() const;

    void setUtilizationRange(Range range);
    Range utilizationRange() const;

    void setAllowDeadTransitions(bool allowDeadTransitions);
    bool allowDeadTransitions() const;

private:
    DoubleRangeWidget* m_iterationsRange;
    DoubleRangeWidget* m_utilizationRange;
    QCheckBox* m_allowDeadTransitions;
};

#endif //SIMULATIONANALYSISMODULE_H
