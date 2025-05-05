// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef ABSTRACTANALYSISMODULE_H
#define ABSTRACTANALYSISMODULE_H

#include <span>
#include <QObject>

namespace ptn::net
{
    struct PetriNet;
}
namespace ptn::modules::decompose
{
    struct DecomposeContext;
}

class QSettings;

class AbstractAnalysisModule {
public:

    using context_type = ptn::modules::decompose::DecomposeContext;

    AbstractAnalysisModule() = default;
    virtual ~AbstractAnalysisModule() = default;

    /* Returns the UI settings editor for this module
     *
     * @param context The decomposition context
     * @return The settings editor
     */
    virtual QWidget* createSettingsEditor(context_type*) const = 0;

    virtual void restoreEditorData(QWidget* editor) = 0;
    virtual void applyEditorData(QWidget* editor) = 0;

    void enable(bool enabled) noexcept;

    bool isEnabled() const noexcept;

    virtual bool canDisable() const noexcept;

    /* Returns the number of configured conditions
     *
     * This needs to know is this module validate conditions
     *
     * @return number of configured conditions
     */
    bool needValidation() const noexcept;

    virtual void saveState(QSettings& settings) const = 0;

    virtual void loadState(const QSettings& settings) = 0;

    /*
     *
     *
     */
    virtual bool run(ptn::net::PetriNet*, std::span<QVariant>, double& metricCost) const = 0;

    /*
     *
     *
     */
    virtual QString displayName() const noexcept = 0;

    /* Returns the count of metrics
     *
     * @return The count of metrics
     */
    virtual int metricCount() const = 0;

    /*
     *
     *
     */
    virtual QString metricName(int) const = 0;

    virtual QMetaType::Type metricType(int) const = 0;

protected:

    virtual bool isNeedValidation() const noexcept = 0;

private:

    bool m_enabled = true;

};

#endif // ABSTRACTANALYSISMODULE_H
