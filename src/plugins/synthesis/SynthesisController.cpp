// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "SynthesisController.h"
#include "SynthesisEditorWidget.h"
#include <editor/GraphicsScene.h>
#include <editor/GraphicsView.h>
#include "SynthesisConfigurator.h"
#include "SynthesisModel.h"
#include "SynthesisWorker.h"
#include "SynthesisWorkerController.h"
#include <ptn/decompose.h>
#include <QHeaderView>
#include <QMessageBox>
#include <QSettings>
#include <Q3DScatter>
#include <QComboBox>
#include <unordered_set>
#include <QTableView>
#include <QItemModelScatterDataProxy>
#include <editor/model/NetModel.h>
#include <corelib/ApplicationProjectController.h>
#include <simulation/SimulationWidget.h>

#include "AbstractAnalysisModule.h"
#include "ClassificationAnalysisModule.h"
#include "ProxyScatterItemModel.h"
#include "SimulationAnalysisModule.h"
#include "StaticAnalysisModule.h"

SynthesisController::SynthesisController(SynthesisEditorWidget* parent)
    : QObject(parent)
    , m_editorWidget(parent)
    , m_workerController(nullptr)
    , m_synthesisTable(new QTableView(parent))
    , m_synthesisModel(new SynthesisModel(this))
    , m_synthesisView(new GraphicsView(parent))
    , m_synthesisScene(new GraphicsScene)
{
    m_synthesisTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_synthesisTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_synthesisTable->verticalHeader()->hide();
    m_synthesisTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_synthesisTable->setModel(m_synthesisModel);

    new SimulationWidget(m_synthesisView);
    m_synthesisView->setScene(m_synthesisScene);

    connect(
        m_synthesisTable->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &SynthesisController::onSynthesisTableSelectionChanged
    );

    m_scatter = new Q3DScatter;
    m_scatter->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    m_scatter->setHorizontalAspectRatio(1.0f);

    m_scatter->axisX()->setTitle("Unspecified");
    m_scatter->axisX()->setTitleVisible(true);
    connect(m_scatter->axisX(), &QValue3DAxis::rangeChanged, this, &SynthesisController::onSegmentRangeChanged);

    m_scatter->axisY()->setTitle("Unspecified");
    m_scatter->axisY()->setTitleVisible(true);
    connect(m_scatter->axisY(), &QValue3DAxis::rangeChanged, this,  &SynthesisController::onSegmentRangeChanged);

    m_scatter->axisZ()->setTitle("Unspecified");
    m_scatter->axisZ()->setTitleVisible(true);
    connect(m_scatter->axisZ(), &QValue3DAxis::rangeChanged, this, &SynthesisController::onSegmentRangeChanged);

    auto proxyModel = new ProxyScatterItemModel(this);
    proxyModel->setSourceModel(m_synthesisModel);

    m_proxy = new QItemModelScatterDataProxy(m_synthesisModel);
    m_proxy->setItemModel(proxyModel);

    m_series = new QScatter3DSeries();
    m_series->setDataProxy(m_proxy);
    m_scatter->addSeries(m_series);

    m_cbAxisX = new QComboBox();
    m_cbAxisY = new QComboBox();
    m_cbAxisZ = new QComboBox();

    connect(proxyModel, &ProxyScatterItemModel::roleNamesChanged, this, &SynthesisController::onRoleNamesChanged);
    connect(m_cbAxisX, &QComboBox::currentIndexChanged, this, &SynthesisController::onComboBoxChanged);
    connect(m_cbAxisY, &QComboBox::currentIndexChanged, this, &SynthesisController::onComboBoxChanged);
    connect(m_cbAxisZ, &QComboBox::currentIndexChanged, this, &SynthesisController::onComboBoxChanged);

    onRoleNamesChanged();
}

SynthesisController::~SynthesisController() { terminateTask(); }

std::vector<std::shared_ptr<AbstractAnalysisModule>>
SynthesisController::requestSynthesisConfiguration(bool& ok)
{
    std::vector<std::shared_ptr<AbstractAnalysisModule>> modules = {
        std::make_shared<StaticAnalysisModule>(),
        std::make_shared<SimulationAnalysisModule>(),
        std::make_shared<ClassificationAnalysisModule>(),
    };

    std::unordered_map<QWidget*, std::shared_ptr<AbstractAnalysisModule>> editorToModule;
    SynthesisConfigurator configurator(m_editorWidget);
    for (auto module : modules) {
        auto editor = module->createSettingsEditor(m_editorWidget->decomposition().get());
        module->restoreEditorData(editor);
        configurator.addEditor(editor, module->displayName(), module->canDisable());
        editorToModule.insert({editor, module});
    }

    if (configurator.exec() != QDialog::Accepted) {
        ok = false;
    } else {
        for (auto [editor, module] : editorToModule) {
            module->applyEditorData(editor);
        }

        // save to file state
        const auto [first, last] = std::ranges::remove_if(modules, [](auto module) {
            return !module->isEnabled();
        });
        modules.erase(first, last);
    }

    return modules;
}

void SynthesisController::terminateTask()
{
    if (m_workerController) {
        disconnect(
            m_workerController,
            &SynthesisWorkerController::programsEvaluated,
            this,
            &SynthesisController::onProgramsEvaluated
        );
        m_workerController->abort();
        m_workerController->deleteLater();
        m_workerController = nullptr;
    }
}

QTableView* SynthesisController::tableView() const noexcept { return m_synthesisTable; }

GraphicsView* SynthesisController::graphicsView() const noexcept { return m_synthesisView; }

Q3DScatter* SynthesisController::scatter3D() const noexcept {
    return m_scatter;
}

void SynthesisController::onStartRequested()
{
    bool ok = true;
    auto modules = requestSynthesisConfiguration(ok);
    if (!ok) {
        return;
    }

    if (ok && m_workerController && m_workerController->isRunning()) {
        // ask for terminate
        auto ask = QMessageBox::warning(
            m_editorWidget,
            "Synthesis",
            "Synthesis is already running. Do you want to terminate it?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (ask == QMessageBox::No) {
            return;
        }
    }

    terminateTask();

    m_roleNames.clear();
    QList<QString> columns;
    for (auto module : modules) {
        for (size_t i = 0; i < module->metricCount(); i++) {
            columns.append(module->metricName(i));

            QMetaType::Type type = module->metricType(i);
            if (type == QMetaType::Int || type == QMetaType::Double) {
                m_roleNames.insert(module->metricName(i), module->metricType(i));
            }
        }
    }

    m_synthesisScene->setModel(nullptr);
    m_synthesisModel->clearPrograms();
    m_synthesisModel->setAdditionalColumns(columns);

    std::shared_ptr<SynthesisContext> context =
        std::make_shared<SynthesisContext>(m_editorWidget->decomposition().get(), modules);
    m_workerController = new SynthesisWorkerController(context, this);
    connect(
        m_workerController,
        &SynthesisWorkerController::programsEvaluated,
        this,
        &SynthesisController::onProgramsEvaluated
    );

    m_workerController->start();
}

void SynthesisController::onProgramsEvaluated(const QList<Evaluated>& array)
{
    m_synthesisModel->appendPrograms(array);
}

void SynthesisController::onSynthesisTableSelectionChanged(
    const QItemSelection& selected,
    const QItemSelection& deselected
)
{
    Q_UNUSED(deselected)
    assert(selected.size() < 2);

    if (selected.empty()) {
        // clear view
        return;
    }

    auto program = m_synthesisModel->program(selected.indexes().first().row());
    auto placesSlice = rust::Slice<u16>(program.places.data(), program.places.size());
    auto transitionsSlice = rust::Slice<u16>(program.transitions.data(), program.transitions.size());
    const auto result = m_editorWidget->decomposition()->eval_program(placesSlice, transitionsSlice);
    auto newModel = new NetModel(result);
    m_synthesisScene->setModel(newModel);
    m_synthesisScene->dotVisualization("dot");
}

void SynthesisController::onScatterRangeChanged(float min, float max) {
    auto s = sender();
    if (!s) {
        return;
    }

    auto axis = qobject_cast<QValue3DAxis*>(s);
    if (!axis) {
        return;
    }

    int segments = floor(max - min);
    axis->setSegmentCount(std::max(1, segments));
    if (segments <= 1) {
        axis->setSubSegmentCount(5);
    }
}

void SynthesisController::onRoleNamesChanged() {
    m_cbAxisX->clear();
    m_cbAxisY->clear();
    m_cbAxisZ->clear();

    auto roles = m_proxy->itemModel()->roleNames();
    roles.removeIf([this](QHash<int, QByteArray>::iterator it) {
        return !m_roleNames.contains(it.value());
    });

    if (roles.isEmpty()) {
        m_cbAxisX->addItem("No value", "");
        m_cbAxisY->addItem("No value", "");
        m_cbAxisZ->addItem("No value", "");
    } else {
        for (auto name : roles) {
            m_cbAxisX->addItem(name, name);
            m_cbAxisY->addItem(name, name);
            m_cbAxisZ->addItem(name, name);
        }
    }

    m_cbAxisX->setCurrentIndex(0);
    m_cbAxisY->setCurrentIndex(std::min(m_cbAxisY->count() - 1, 1));
    m_cbAxisZ->setCurrentIndex(std::min(m_cbAxisZ->count() - 1, 2));
}

void SynthesisController::onComboBoxChanged() {
    QString axisX = m_cbAxisX->currentData().toString();
    QString axisY = m_cbAxisY->currentData().toString();
    QString axisZ = m_cbAxisZ->currentData().toString();

    m_scatter->axisX()->setTitle(axisX);
    m_scatter->axisY()->setTitle(axisY);
    m_scatter->axisZ()->setTitle(axisZ);

    m_proxy->remap(axisX, axisY, axisZ, "");
}

void SynthesisController::onSegmentRangeChanged(float min, float max) {
    auto axis = qobject_cast<QValue3DAxis*>(sender());
    if (!axis) {
        qDebug() << "NOT AXIS";
        return;
    }

    int segments = floor(max - min);
    axis->setSegmentCount(std::max(1, std::min(10, segments)));
    if (segments <= 1) {
        axis->setSubSegmentCount(5);
    }
}
