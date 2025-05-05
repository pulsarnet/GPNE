// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef CLASSIFICATIONANALYSISMODULE_H
#define CLASSIFICATIONANALYSISMODULE_H

#include "AbstractAnalysisModule.h"
#include <QWidget>

class QListWidget;

class ClassificationAnalysisModule : public AbstractAnalysisModule {
public:
    ClassificationAnalysisModule();
    virtual ~ClassificationAnalysisModule() = default;

    QWidget* createSettingsEditor(context_type *) const override;

    void restoreEditorData(QWidget *editor) override;
    void applyEditorData(QWidget *editor) override;

    void saveState(QSettings &settings) const override;
    void loadState(const QSettings &settings) override;

    bool run(ptn::net::PetriNet *, std::span<QVariant>, double &metricCost) const override;

    QString displayName() const noexcept override;

    int metricCount() const override;
    QString metricName(int) const override;
    QMetaType::Type metricType(int) const override;

protected:
    bool isNeedValidation() const noexcept override;
};

class ClassificationSettingsEditor : public QWidget {
    Q_OBJECT
public:
    ClassificationSettingsEditor(QWidget* parent = nullptr);
    virtual ~ClassificationSettingsEditor() = default;

private:
    QListWidget* m_netClass;
};


#endif //CLASSIFICATIONANALYSISMODULE_H
