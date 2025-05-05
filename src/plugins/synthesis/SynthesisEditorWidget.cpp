// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisEditorWidget.h"

#include <corelib/Project.h>
#include <editor/GraphicsScene.h>
#include <editor/GraphicsView.h>
#include <iomatrix/NamedMatrixModel.h>
#include <ptn/decompose.h>
#include <Q3DScatter>
#include <QGridLayout>
#include <QHeaderView>
#include <ranges>
#include <DockManager.h>
#include <DockAreaWidget.h>
#include <QComboBox>
#include <QToolBar>
#include <QTableView>
#include "SynthesisController.h"
#include "SynthesisDocument.h"

inline size_t qHash(const QVector3D& v)
{
    return qHash(QString("%1x%2x%3").arg(v.x()).arg(v.y()).arg(v.z()));
}

SynthesisEditorWidget::SynthesisEditorWidget(SynthesisDocument *document, QWidget *parent)
    : QWidget(parent)
    , m_document(document)
    , m_synthesisController(new SynthesisController(this))
{
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, true);
    m_dockManager = new ads::CDockManager(this);

    initLinearBaseFragments();
    initPrimitiveNet();
    initTensorView();
    initPlot();
    initSynthesisView();

    auto lbf_area = m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_linearBaseFragmentsView);
    m_dockManager->addDockWidgetTab(ads::LeftDockWidgetArea, m_primitiveNetView);
    m_dockManager->addDockWidgetTab(ads::LeftDockWidgetArea, m_tensorDockWidget);
    auto plot_area = m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_plotWidget);
    m_dockManager->addDockWidget(ads::BottomDockWidgetArea, m_synthesisTableDockWidget, plot_area);
    m_dockManager->addDockWidget(ads::BottomDockWidgetArea, m_synthesisViewDockWidget, lbf_area);

    setMouseTracking(true);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_dockManager);
    layout()->setContentsMargins(QMargins());
}


SynthesisEditorWidget::~SynthesisEditorWidget()
{

}

SynthesisDocument::shared_decomposition_t SynthesisEditorWidget::decomposition() {
    return m_document->decomposition();
}

SynthesisController * SynthesisEditorWidget::controller() const noexcept {
    return m_synthesisController;
}

void SynthesisEditorWidget::addView(ads::CDockWidget* widget)
{
}

void SynthesisEditorWidget::initLinearBaseFragments()
{
    auto linearBaseFragmentsModel = new NetModel(decomposition()->lbf());
    auto linearBaseFragmentsScene = new GraphicsScene();
    linearBaseFragmentsScene->setModel(linearBaseFragmentsModel);
    linearBaseFragmentsScene->dotVisualization("dot");
    auto linearBaseFragmentsView = new GraphicsView(nullptr);
    linearBaseFragmentsView->setScene(linearBaseFragmentsScene);
//    linearBaseFragmentsView->setToolBoxVisibility(false);
    m_linearBaseFragmentsView = m_dockManager->createDockWidget(tr("LBF"));
    m_linearBaseFragmentsView->setWidget(linearBaseFragmentsView);
    // m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, m_linearBaseFragmentsView);
    addView(m_linearBaseFragmentsView);
}

void SynthesisEditorWidget::initPrimitiveNet()
{
    auto primitiveModel = new NetModel(decomposition()->primitive());
    auto primitiveNetScene = new GraphicsScene();
    primitiveNetScene->setModel(primitiveModel);
    primitiveNetScene->dotVisualization("dot");
    auto primitiveNetView = new GraphicsView(nullptr);
    primitiveNetView->setScene(primitiveNetScene);
 //   primitiveNetView->setToolBoxVisibility(false);
    m_primitiveNetView = m_dockManager->createDockWidget(tr("Primitive view"));
    m_primitiveNetView->setWidget(primitiveNetView);
    // m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, m_primitiveNetView);
    addView(m_primitiveNetView);
}

void SynthesisEditorWidget::initPlot()
{
    m_plotWidget = m_dockManager->createDockWidget(QObject::tr("Surface"));
    m_plotWidget->setWidget(createWindowContainer(m_synthesisController->scatter3D()));
    m_plotWidget->createDefaultToolBar();
    m_plotWidget->toolBar()->addWidget(m_synthesisController->cbAxisX());
    m_plotWidget->toolBar()->addWidget(m_synthesisController->cbAxisY());
    m_plotWidget->toolBar()->addWidget(m_synthesisController->cbAxisZ());
    // m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, m_plotWidget);
    addView(m_plotWidget);
}

void SynthesisEditorWidget::initTensorView()
{
    auto matrixModel = new NamedMatrixModel<f64>();
    matrixModel->setMatrix(std::move(decomposition()->tensor()));

    auto tableView = new QTableView;
    tableView->setModel(matrixModel);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_tensorDockWidget = m_dockManager->createDockWidget("Tensor");
    m_tensorDockWidget->setWidget(tableView);
    // m_dockManager->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, tableViewDockWidget);
    addView(m_tensorDockWidget);
}

void SynthesisEditorWidget::initSynthesisView()
{
    m_synthesisTableDockWidget = m_dockManager->createDockWidget("Synthesis Table");
    m_synthesisTableDockWidget->setWidget(m_synthesisController->tableView());
    // m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, m_synthesisTableDockWidget);

    m_synthesisViewDockWidget = m_dockManager->createDockWidget("Synthesis View");
    m_synthesisViewDockWidget->setWidget(m_synthesisController->graphicsView());
    // m_dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, m_synthesisViewDockWidget);

    addView(m_synthesisTableDockWidget);
    addView(m_synthesisViewDockWidget);
}

