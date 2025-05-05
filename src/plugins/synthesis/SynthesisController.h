// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef SYNTHESISCONTROLLER_H
#define SYNTHESISCONTROLLER_H

#include "SynthesisWorker.h"
#include <QHash>

class QItemModelScatterDataProxy;
class QScatter3DSeries;
class GraphicsScene;
class SynthesisModel;
class SynthesisWorkerController;
class QItemSelection;
class QComboBox;
class Q3DScatter;
class GraphicsView;
class QTableView;
class SynthesisEditorWidget;

class SynthesisController : public QObject {

    Q_OBJECT

public:
    explicit SynthesisController(SynthesisEditorWidget* parent);
    ~SynthesisController();

    std::vector<std::shared_ptr<AbstractAnalysisModule>> requestSynthesisConfiguration(bool& ok);
    void terminateTask();

    QTableView* tableView() const noexcept;
    GraphicsView* graphicsView() const noexcept;
    Q3DScatter* scatter3D() const noexcept;
    QComboBox* cbAxisX() const noexcept { return m_cbAxisX; }
    QComboBox* cbAxisY() const noexcept { return m_cbAxisY; }
    QComboBox* cbAxisZ() const noexcept { return m_cbAxisZ; }

public slots:

    void onStartRequested();
    void onProgramsEvaluated(const QList<Evaluated>& array);
    void onSynthesisTableSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void onScatterRangeChanged(float min, float max);

    void onRoleNamesChanged();
    void onComboBoxChanged();

private slots:
    void onSegmentRangeChanged(float min, float max);

private:
    SynthesisEditorWidget* m_editorWidget;
    SynthesisWorkerController* m_workerController;

    QTableView* m_synthesisTable;
    SynthesisModel* m_synthesisModel;
    GraphicsView* m_synthesisView;
    GraphicsScene* m_synthesisScene;

    Q3DScatter* m_scatter;
    QScatter3DSeries* m_series;
    QItemModelScatterDataProxy* m_proxy;
    QComboBox* m_cbAxisX;
    QComboBox* m_cbAxisY;
    QComboBox* m_cbAxisZ;

    QHash<QString, QMetaType::Type> m_roleNames;
};

#endif // SYNTHESISCONTROLLER_H
