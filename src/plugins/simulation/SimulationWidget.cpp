// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SimulationWidget.h"

#include <QApplication>
#include <editor/GraphicsScene.h>
#include <editor/GraphicsView.h>
#include <editor/registry/IVertex.h>
#include <editor/registry/Place.h>
#include <ptn/net.h>
#include <ptn/simulation.h>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QStyleOptionFrame>
#include <QPushButton>
#include <QTimer>

SimulationWidget::SimulationWidget(GraphicsView* parent)
    : QFrame(parent)
    , m_timer(new QTimer(this))
    , m_state(Stopped)
    , m_simulation(nullptr)
{
    parent->installEventFilter(this);
    setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    setMinimumSize(315, 48);

    setFrameStyle(StyledPanel | Plain);
    // shadow
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 150));
    setGraphicsEffect(shadow);

    // layout
    auto* layout = new QHBoxLayout(this);
    layout->setSpacing(3);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    m_runButton = new QPushButton(this);
    m_runButton->setIcon(QIcon(":/images/simulation/run.svg"));
    m_runButton->setMaximumSize(30, 30);
    m_runButton->setMinimumSize(30, 30);
    m_runButton->setCheckable(true);
    layout->addWidget(m_runButton);

    m_pauseButton = new QPushButton(this);
    m_pauseButton->setIcon(QIcon(":/images/simulation/pause.svg"));
    m_pauseButton->setMaximumSize(30, 30);
    m_pauseButton->setMinimumSize(30, 30);
    m_pauseButton->setCheckable(true);
    layout->addWidget(m_pauseButton);

    m_stopButton = new QPushButton(this);
    m_stopButton->setIcon(QIcon(":/images/simulation/stop.svg"));
    m_stopButton->setMaximumSize(30, 30);
    m_stopButton->setMinimumSize(30, 30);
    layout->addWidget(m_stopButton);

    m_stepButton = new QPushButton(this);
    m_stepButton->setIcon(QIcon(":/images/simulation/step.svg"));
    m_stepButton->setMaximumSize(30, 30);
    m_stepButton->setMinimumSize(30, 30);
    layout->addWidget(m_stepButton);

    m_speedDownButton = new QPushButton(this);
    m_speedDownButton->setIcon(QIcon(":/images/simulation/speeddown.svg"));
    m_speedDownButton->setMaximumSize(30, 30);
    m_speedDownButton->setMinimumSize(30, 30);
    layout->addWidget(m_speedDownButton);

    m_speedUpButton = new QPushButton(this);
    m_speedUpButton->setIcon(QIcon(":/images/simulation/speedup.svg"));
    m_speedUpButton->setMaximumSize(30, 30);
    m_speedUpButton->setMinimumSize(30, 30);
    layout->addWidget(m_speedUpButton);

    m_cycleCounterLabel = new QLabel(this);
    m_cycleCounterLabel->setFont(QFont("JetBrains Mono", 14));
    layout->addWidget(m_cycleCounterLabel);

    updateLabel();

    connect(m_runButton, &QPushButton::clicked, this, &SimulationWidget::runSimulation);
    connect(m_pauseButton, &QPushButton::clicked, this, &SimulationWidget::pauseSimulation);
    connect(m_stopButton, &QPushButton::clicked, this, &SimulationWidget::stopSimulation);
    connect(m_stepButton, &QPushButton::clicked, this, &SimulationWidget::stepSimulation);
    connect(m_speedUpButton, &QPushButton::clicked, this, &SimulationWidget::speedUp);
    connect(m_speedDownButton, &QPushButton::clicked, this, &SimulationWidget::speedDown);

    updateButtonState();

    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &SimulationWidget::simulate);
}

void SimulationWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QStyleOptionFrame opt;
    opt.initFrom(this);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);
}

SimulationWidget::State SimulationWidget::state() const { return m_state; }

void SimulationWidget::runSimulation()
{
    if (m_state != State::Paused) {
        if (!initSimulation()) {
            updateButtonState();
            return;
        }
    }

    m_timer->start();
    m_state = State::Running;
    updateButtonState();
}

void SimulationWidget::pauseSimulation()
{
    m_timer->stop();

    m_state = State::Paused;
    updateButtonState();
    updateScene();
}

void SimulationWidget::stopSimulation()
{
    m_timer->stop();
    this->cancelSimulation();

    m_state = State::Stopped;
    updateButtonState();
    emit stopped();
}

void SimulationWidget::stepSimulation()
{
    if (m_state != State::Paused) {
        if (!initSimulation()) {
            updateButtonState();
            return;
        }
    }
    simulate();

    m_state = State::Paused;
    updateButtonState();
}

void SimulationWidget::speedUp()
{
    m_timer->setInterval(m_timer->interval() / 2);
}

void SimulationWidget::speedDown()
{
    m_timer->setInterval(m_timer->interval() * 2);
}

bool SimulationWidget::eventFilter(QObject *watched, QEvent *event) {
    if (watched != this) {
        if (event->type() == QEvent::Resize) {
            resizeEvent(nullptr);
        }
    }
    return QFrame::eventFilter(watched, event);
}

void SimulationWidget::resizeEvent(QResizeEvent *event) {
    auto g = geometry();
    // centered bottom
    setGeometry(
        (parentWidget()->rect().width() - g.width()) / 2,
        parentWidget()->rect().height() - g.height() - 10,
        g.width(),
        g.height()
    );
    QFrame::resizeEvent(event);
}

ptn::modules::simulation::Simulation* SimulationWidget::simulation()
{
    return m_simulation;
}

GraphicsScene* SimulationWidget::scene()
{
    auto p = qobject_cast<GraphicsView*>(parent());
    return qobject_cast<GraphicsScene*>(p->scene());
}

void SimulationWidget::updateButtonState()
{
    switch (m_state) {
    case Stopped:
        m_runButton->setChecked(false);
        m_pauseButton->setChecked(false);

        m_runButton->setEnabled(true);
        m_pauseButton->setEnabled(false);
        m_stopButton->setEnabled(false);
        m_stepButton->setEnabled(true);
        m_speedDownButton->setEnabled(true);
        m_speedUpButton->setEnabled(true);
        break;
    case Running:
        m_pauseButton->setChecked(false);

        m_runButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
        m_stopButton->setEnabled(true);
        m_stepButton->setEnabled(false);
        m_speedDownButton->setEnabled(true);
        m_speedUpButton->setEnabled(true);
        break;
    case Paused:

        m_runButton->setChecked(false);

        m_runButton->setEnabled(true);
        m_pauseButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_stepButton->setEnabled(true);
        m_speedDownButton->setEnabled(true);
        m_speedUpButton->setEnabled(true);
        break;
    }
}

void SimulationWidget::updateLabel()
{
    size_t cycles = simulation() ? simulation()->cycles() : 0;
    m_cycleCounterLabel->setText("Cycles: " + QString::number(cycles));
}

void SimulationWidget::updateScene()
{
    if (m_simulationModel && m_simulation) {
        for (const auto& vertex : m_simulationModel->vertices()) {
            if (vertex->classId() == IVertex::Place) {
                vertex::VertexIndex id(vertex->vertexType(), vertex->id());
                static_cast<Place*>(vertex)->setMarkers(m_simulation->markers_at(id));
            }
        }
    }
}

bool SimulationWidget::initSimulation()
{
    if (simulation()) {
        return true;
    }

    m_oldModel = scene()->model();
    if (!m_oldModel) {
        return false;
    }

    m_simulationModel = m_oldModel->copy();
    scene()->setModel(m_simulationModel);

    m_simulation = ptn::modules::simulation::Simulation::init(m_simulationModel->net());
    updateLabel();

    emit started();

    return true;
}

void SimulationWidget::simulate()
{
    int fired = simulation()->step();
    if (fired == 0) {
        this->pauseSimulation();
        return;
    }

    updateScene();
    updateLabel();
}

void SimulationWidget::cancelSimulation()
{
    if (m_simulation) {
        m_simulation->drop();
    }
    m_simulation = nullptr;

    if (m_oldModel) {
        scene()->setModel(m_oldModel);
        m_oldModel = nullptr;
    }

    if (m_simulationModel) {
        m_simulationModel->deleteLater();
        m_simulationModel = nullptr;
    }

    updateLabel();
}
