// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_REACHABILITY_WINDOW_H
#define GPNE_REACHABILITY_WINDOW_H

#include <QWidget>

class QToolBar;
class PetriNetDocument;
class ReachabilityDocument;
class TaskManager;
class ReachabilityGraphScene;
class NetModel;

namespace ptn::net
{
    struct PetriNet;
}

namespace ptn::modules::reachability
{
    struct Reachability;
}

class ReachabilityView;

class ReachabilityWindow : public QWidget {

    Q_OBJECT

public:
    explicit ReachabilityWindow(ReachabilityDocument*, QWidget* parent = nullptr);
    ~ReachabilityWindow() override;

public slots:

    void onGraphComplete();
    void onFitInView();

protected:

    void updateScene();

private:
    ReachabilityDocument* m_document;

    QToolBar* m_toolbar;
    ReachabilityView* m_view;
    ReachabilityGraphScene* m_scene;

};

#endif // GPNE_REACHABILITY_WINDOW_H
