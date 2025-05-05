// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "ReachabilityWindow.h"
#include <corelib/ActionManager.h>
#include <corelib/task/TaskManager.h>
#include <DockAreaWidget.h>
#include <editor/model/NetModel.h>
#include <projectexplorer/Constants.h>
#include <QGridLayout>
#include <QToolBar>

#include "BuildReachabilityGraphTask.h"
#include "Constants.h"
#include "ReachabilityDocument.h"
#include "ReachabilityGraphScene.h"
#include "ReachabilityView.h"

ReachabilityWindow::ReachabilityWindow(ReachabilityDocument* document, QWidget* parent)
    : QWidget(parent)
    , m_document(document)
{
    m_toolbar = ActionManager::group(Constants::G_REACHABILITY_TOOLBAR)->createToolBar();
    m_toolbar->setParent(this);

    m_scene= new ReachabilityGraphScene();
    m_view = new ReachabilityView(this);
    m_view->setScene(m_scene);

    connect(document, &ReachabilityDocument::contentChanged, this, &ReachabilityWindow::onGraphComplete);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_toolbar);
    layout()->addWidget(m_view);
    layout()->setContentsMargins(0, 0, 0, 0);

    onGraphComplete();
}

void ReachabilityWindow::updateScene() {
    m_scene->removeAll();

    auto graph = m_document->graph();
    if (!graph) {
        return;
    }

    auto markings = graph->marking();
    auto indexes = graph->positions();
    auto headers = QList(indexes.begin(), indexes.end());

    for (auto& marking : markings) {
        auto markers = marking.values();
        // create node from data
        auto list = QList(markers.begin(), markers.end());
        m_scene->addNode(list, marking.type(), headers);
    }

    for (int i = 0; i < markings.size(); i++) {
        const auto next = markings[i].next();
        for (const auto& [idx, transition] : next) {
            m_scene->addEdge(m_scene->node_at(i), m_scene->node_at(idx), transition);
        }
    }
}

void ReachabilityWindow::onGraphComplete()
{
    updateScene();
    m_scene->updateLayout();
    m_view->fitInView();
}

void ReachabilityWindow::onFitInView() {
    m_view->fitInView();
}

ReachabilityWindow::~ReachabilityWindow()
{

}

