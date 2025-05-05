// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_SIMULATIONWIDGET_H
#define GPNE_SIMULATIONWIDGET_H

#include <QFrame>

class NetModel;
class QLabel;
class QPushButton;
class GraphicsScene;
class GraphicsView;

namespace ptn::modules::simulation
{
    struct Simulation;
}

class SimulationWidget : public QFrame {

    Q_OBJECT

public:
    enum State
    {
        Running = 0,
        Paused,
        Stopped
    };

    explicit SimulationWidget(GraphicsView* parent);
    ~SimulationWidget() = default;

    void paintEvent(QPaintEvent* event) override;

    State state() const;

public slots:

    void runSimulation();
    void pauseSimulation();
    void stopSimulation();
    void stepSimulation();
    void speedUp();
    void speedDown();

    bool eventFilter(QObject *watched, QEvent *event) override;

signals:

    void started();
    void stopped();

protected:

    void resizeEvent(QResizeEvent *event) override;

private:
    ptn::modules::simulation::Simulation* simulation();
    GraphicsScene* scene();

    void updateButtonState();
    void updateLabel();
    void updateScene();

    bool initSimulation();
    void simulate();
    void cancelSimulation();

private:
    QTimer* m_timer;

    QPushButton* m_runButton;
    QPushButton* m_pauseButton;
    QPushButton* m_stopButton;
    QPushButton* m_stepButton;
    QPushButton* m_speedUpButton;
    QPushButton* m_speedDownButton;

    QLabel* m_cycleCounterLabel;

    State m_state;

    ptn::modules::simulation::Simulation* m_simulation;
    NetModel* m_oldModel = nullptr;
    NetModel* m_simulationModel = nullptr;
};

#endif // GPNE_SIMULATIONWIDGET_H
