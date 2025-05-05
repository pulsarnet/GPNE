// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_SYNTHESISEDITORWIDGET_H
#define GPNE_SYNTHESISEDITORWIDGET_H

#include "SynthesisDocument.h"
#include "AbstractAnalysisModule.h"
#include <QWidget>

namespace ads
{
    class CDockWidget;
    class CDockManager;
}
namespace ptn::modules::decompose
{
    struct DecomposeContext;
}

namespace ptn::net
{
    struct PetriNet;
}

class QItemModelScatterDataProxy;
class SynthesisController;
class SynthesisDocument;
class Q3DScatter;
class QScatter3DSeries;

class SynthesisEditorWidget : public QWidget {

    Q_OBJECT

public:
    explicit SynthesisEditorWidget(SynthesisDocument* document, QWidget* parent = nullptr);
    ~SynthesisEditorWidget();

    SynthesisDocument::shared_decomposition_t decomposition();
    SynthesisController* controller() const noexcept;

protected:
    void addView(ads::CDockWidget* widget);

    void initTreeViewList();
    void initLinearBaseFragments();
    void initPrimitiveNet();
    void initPlot();
    void initTensorView();
    void initSynthesisView();

    std::vector<std::shared_ptr<AbstractAnalysisModule>> requestSynthesisConfiguration(bool& ok);

private:
    SynthesisDocument* m_document;

    ads::CDockManager* m_dockManager;
    ads::CDockWidget* m_linearBaseFragmentsView;
    ads::CDockWidget* m_primitiveNetView;
    ads::CDockWidget* m_plotWidget;
    ads::CDockWidget* m_synthesisViewDockWidget;
    ads::CDockWidget* m_synthesisTableDockWidget;
    ads::CDockWidget* m_tensorDockWidget;

    Q3DScatter* m_scatter;
    QItemModelScatterDataProxy* m_proxy;
    QScatter3DSeries* m_series;

    SynthesisController* m_synthesisController;
};

#endif // GPNE_SYNTHESISEDITORWIDGET_H
