// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#include "EditorWidget.h"
#include <corelib/Project.h>
#include <corelib/PropertyMap.h>
#include <DockAreaWidget.h>
#include <editor/GraphicsScene.h>
#include <editor/GraphicsSceneActions.h>
#include <editor/GraphicsView.h>
#include <editor/model/NetModel.h>
#include <editor/registry/IEdge.h>
#include <editor/registry/IVertex.h>
#include <editor/registry/Registry.h>
#include <editor/topology/TopologyWidget.h>
#include <QActionGroup>
#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include <qmetaobject.h>
#include <QSplitter>
#include <QStackedLayout>
#include <QStackedWidget>
#include <qteditorfactory.h>
#include <QToolBar>
#include <QToolBox>
#include <qtpropertymanager.h>
#include <QTreeWidget>
#include <QtTreePropertyBrowser>
#include <qtvariantproperty.h>

#include "toolbox/ToolBox.h"

using namespace ads;

struct ObjectPropertyInfo {
    QtBrowserItem* groupItem{nullptr};
    PropertyMap propertyMap;
    QMap<QLatin1String, QMetaProperty> propertyMetaMap;
};

EditorWidget::EditorWidget(NetModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
    , m_dockManager(new CDockManager(this))
{
    /************************* CREATE VIEW *************************/
    initGraphicsView();
    initTopologyView();
    initPropertyEditor();

    /************************* CREATE LAYOUT *************************/
    setLayout(new QGridLayout(this));
    layout()->addWidget(m_dockManager);
    layout()->setContentsMargins(0, 0, 0, 0);

    connect(m_model, &NetModel::selectionChanged, this, &EditorWidget::onModelSelectionChanged);
    connect(m_model, &NetModel::vertexChanged, this, &EditorWidget::onVertexChanged);
    connect(m_model, &NetModel::edgeChanged, this, &EditorWidget::onEdgeChanged);
}

GraphicsView* EditorWidget::view() const { return static_cast<GraphicsView*>(m_view->widget()); }

void EditorWidget::onVertexPropertyChanged(QtProperty* property, const QVariant& value)
{
    if (!m_currentVertex) {
        return;
    }

    auto it = m_vertexPropertyGroups.find(m_currentVertex->typeId());
    if (it == m_vertexPropertyGroups.end()) {
        return;
    }

    auto info = it.value();

    auto propId = info->propertyMap.id(property);
    if (propId.isEmpty()) {
        return;
    }

    auto metaIt = info->propertyMetaMap.find(propId);
    if (metaIt == info->propertyMetaMap.end()) {
        return;
    }

    metaIt.value().writeOnGadget(m_currentVertex, value);
}

void EditorWidget::onEdgePropertyChanged(QtProperty* property, const QVariant& value)
{
    if (!m_currentEdge) {
        return;
    }

    if (property == m_directionChooser->property()) {
        int idx = value.toInt();
        if (idx == 0) {
            m_currentEdge->setDirections(IEdge::PlaceToTransition);
        } else if (idx == 1) {
            m_currentEdge->setDirections(IEdge::TransitionToPlace);
        } else if (idx == 2) {
            m_currentEdge->setDirections(IEdge::PlaceToTransition | IEdge::TransitionToPlace);
        }
    } else if (property == m_weightPT->property()) {
        // TODO: editor check value
        if (const int weight = value.toInt(); weight > 0) {
            m_currentEdge->setWeight(weight, IEdge::PlaceToTransition);
        }
    } else if (property == m_weightTP->property()) {
        // TODO: editor check value
        if (const int weight = value.toInt(); weight > 0) {
            m_currentEdge->setWeight(weight, IEdge::TransitionToPlace);
        }
    }
}

void EditorWidget::onModelSelectionChanged()
{
    auto selectedVertices = m_model->selectedVertices();
    auto selectedEdges = m_model->selectedEdges();
    if (selectedVertices.length() + selectedEdges.length() == 1) {
        if (selectedVertices.length() == 1) {
            m_currentVertex = selectedVertices.first();
            m_currentEdge = nullptr;
        } else {
            m_currentEdge = selectedEdges.first();
            m_currentVertex = nullptr;
        }
    } else {
        m_currentVertex = nullptr;
        m_currentEdge = nullptr;
    }

    updateVisibility();
    onVertexChanged(m_currentVertex);
    onEdgeChanged(m_currentEdge);
}

void EditorWidget::onVertexChanged(IVertex* vertex)
{
    if (!vertex || vertex != m_currentVertex) {
        return;
    }

    auto groupIt = m_vertexPropertyGroups.find(vertex->typeId());
    if (groupIt == m_vertexPropertyGroups.end()) {
        qWarning() << "No properties for type " << vertex->typeId();
        return;
    }

    auto group = groupIt.value();
    for (auto [id, meta] : group->propertyMetaMap.asKeyValueRange()) {
        auto prop = group->propertyMap.property(id);
        if (prop->propertyManager() == m_vertexPropertyManager) {
            m_vertexPropertyManager->setValue(prop, meta.readOnGadget(m_currentVertex));
        } else if (prop->propertyManager() == m_intPropertyManager) {
            m_intPropertyManager->setValue(prop, meta.readOnGadget(m_currentVertex).toInt());
        }
    }
}

void EditorWidget::onEdgeChanged(IEdge* edge)
{
    if (!edge || edge != m_currentEdge) {
        return;
    }

    bool edgeAllowsPtoT = m_currentEdge && m_currentEdge->allowDirection(IEdge::PlaceToTransition);
    bool edgeAllowsTtoP = m_currentEdge && m_currentEdge->allowDirection(IEdge::TransitionToPlace);

    m_directionChooser->property()->setEnabled(edgeAllowsPtoT && edgeAllowsTtoP);

    int current = 0;
    if (edge->directions().testFlags(IEdge::PlaceToTransition | IEdge::TransitionToPlace)) {
        current = 2;
    } else if (edge->directions().testFlag(IEdge::TransitionToPlace)) {
        current = 1;
    }
    m_directionPropertyManager->setValue(m_directionChooser->property(), current);

    if (m_currentEdge) {
        m_intPropertyManager->setValue(
            m_weightPT->property(),
            m_currentEdge->weight(IEdge::PlaceToTransition)
        );
        m_weightPT->property()->setEnabled(
            m_currentEdge->directions().testFlag(IEdge::PlaceToTransition)
        );

        m_intPropertyManager->setValue(
            m_weightTP->property(),
            m_currentEdge->weight(IEdge::TransitionToPlace)
        );
        m_weightTP->property()->setEnabled(
            m_currentEdge->directions().testFlag(IEdge::TransitionToPlace)
        );
    }
}


void EditorWidget::onMatrixWindowClose(QWidget* window)
{
    Q_UNUSED(window);
    //m_IOMatrixWindow = nullptr;
}

void EditorWidget::initGraphicsView()
{
    auto scene = new GraphicsScene();
    scene->setModel(m_model);

    auto view = new GraphicsView();
    view->setScene(scene);

    m_view = m_dockManager->createDockWidget(tr("Modeling"));
    m_view->setWidget(view);
    m_view->setFeature(CDockWidget::NoTab, true);

    auto area = m_dockManager->setCentralWidget(m_view);
    area->setAllowedAreas(NoDockWidgetArea);
}

void EditorWidget::initTopologyView()
{
    auto topologyWidget = new TopologyWidget;
    topologyWidget->setNetModel(m_model);

    m_topologyView = m_dockManager->createDockWidget(tr("Topology View"));
    m_topologyView->setWidget(topologyWidget);
    auto area = m_dockManager->addDockWidget(DockWidgetArea::RightDockWidgetArea, m_topologyView);
    area->setAllowedAreas(BottomDockWidgetArea | TopDockWidgetArea);
    area->setMinimumWidth(280);

    //addToggleViewAction(m_topologyView->toggleViewAction());
}

void EditorWidget::initPropertyEditor()
{
    m_vertexPropertyManager = new QtVariantPropertyManager(this);
    m_edgePropertyManager = new QtVariantPropertyManager(this);
    m_directionPropertyManager = new QtEnumPropertyManager(this);
    m_intPropertyManager = new QtIntPropertyManager(this);
    m_propertyBrowser = new QtTreePropertyBrowser;

    QtVariantEditorFactory* variantFactory = new QtVariantEditorFactory();
    QtEnumEditorFactory* enumEditorFactory = new QtEnumEditorFactory();
    QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory();
    m_propertyBrowser->setFactoryForManager(m_vertexPropertyManager, variantFactory);
    m_propertyBrowser->setFactoryForManager(m_edgePropertyManager, variantFactory);
    m_propertyBrowser->setFactoryForManager(m_directionPropertyManager, enumEditorFactory);
    m_propertyBrowser->setFactoryForManager(m_intPropertyManager, spinBoxFactory);
    m_propertyBrowser->setRootIsDecorated(false);

    /************ VERTEICES PROPERTY INIT *******************/
    for (auto vertex : Registry::vertices()) {
        ObjectPropertyInfo* objectInfo = new ObjectPropertyInfo;
        QtVariantProperty* group = m_vertexPropertyManager->addProperty(
            QtVariantPropertyManager::groupTypeId(),
            tr(vertex->typeId())
        );
        auto info = vertex->metaObject();
        for (int i = 0; i < info->propertyCount(); i++) {
            auto propInfo = info->property(i);
            QtProperty* property = nullptr;
            if (m_vertexPropertyManager->isPropertyTypeSupported(propInfo.typeId())) {
                property = m_vertexPropertyManager->addProperty(propInfo.typeId(), propInfo.name());
            } else if (propInfo.typeId() == QMetaType::Type::ULongLong) {
                property = m_intPropertyManager->addProperty(propInfo.name());
                m_intPropertyManager->setMinimum(property, 0);
            }

            if (property == nullptr) {
                qWarning() << std::format("Skip property. Type '{}' unsupported.", propInfo.typeName());
                continue;
            }

            property->setEnabled(propInfo.isWritable());
            group->addSubProperty(property);
            objectInfo->propertyMap.addProperty(property, QLatin1StringView(propInfo.name()));
            objectInfo->propertyMetaMap.insert(QLatin1StringView(propInfo.name()), propInfo);
        }
        objectInfo->groupItem = m_propertyBrowser->addProperty(group);
        m_vertexPropertyGroups.insert(vertex->typeId(), objectInfo);
    }

    /************ EDGES PROPERTY INIT *******************/
    auto direction = m_directionPropertyManager->addProperty(tr("direction"));
    m_directionPropertyManager->setEnumNames(
        direction,
        QStringList({"PtoT", "TtoP", "Bidirectional"})
    );
    m_directionChooser = m_propertyBrowser->addProperty(direction);

    auto weightPT = m_intPropertyManager->addProperty(tr("PtoT weight"));
    m_intPropertyManager->setMinimum(weightPT, 0);
    m_intPropertyManager->setValue(weightPT, 1);
    m_weightPT = m_propertyBrowser->addProperty(weightPT);

    auto weightTP = m_intPropertyManager->addProperty(tr("TtoP weight"));
    m_intPropertyManager->setMinimum(weightTP, 0);
    m_intPropertyManager->setValue(weightTP, 1);
    m_weightTP = m_propertyBrowser->addProperty(weightTP);

    /************ DOCK WIDGET *******************/
    m_propertyView = m_dockManager->createDockWidget("Editor View");
    m_propertyView->setWidget(m_propertyBrowser);
    m_propertyView->setFeature(CDockWidget::DockWidgetFloatable, false);
    m_dockManager->addDockWidget(
        DockWidgetArea::BottomDockWidgetArea,
        m_propertyView,
        m_topologyView->dockAreaWidget()
    );
    m_propertyView->dockAreaWidget()->setMinimumWidth(280);

    connect(
        m_vertexPropertyManager,
        &QtVariantPropertyManager::valueChanged,
        this,
        &EditorWidget::onVertexPropertyChanged
    );
    connect(
        m_edgePropertyManager,
        &QtVariantPropertyManager::valueChanged,
        this,
        &EditorWidget::onEdgePropertyChanged
    );
    connect(
        m_directionPropertyManager,
        &QtEnumPropertyManager::valueChanged,
        this,
        &EditorWidget::onEdgePropertyChanged
    );
    connect(
        m_intPropertyManager,
        &QtIntPropertyManager::valueChanged,
        this,
        &EditorWidget::onEdgePropertyChanged
    );
    connect(
        m_intPropertyManager,
        &QtIntPropertyManager::valueChanged,
        this,
        &EditorWidget::onVertexPropertyChanged
    );

    updateVisibility();

    //addToggleViewAction(m_propertyView->toggleViewAction());
}

void EditorWidget::updateVisibility()
{
    bool hasCurrentVertex = m_currentVertex;
    for (auto [key, value] : m_vertexPropertyGroups.asKeyValueRange()) {
        m_propertyBrowser->setItemVisible(
            value->groupItem,
            hasCurrentVertex && key == m_currentVertex->typeId()
        );
    }

    bool hasCurrentEdge = m_currentEdge;
    bool edgeHasWeight = hasCurrentEdge && m_currentEdge->hasWeight();

    m_propertyBrowser->setItemVisible(m_directionChooser, hasCurrentEdge);
    m_propertyBrowser->setItemVisible(m_weightPT, edgeHasWeight);
    m_propertyBrowser->setItemVisible(m_weightTP, edgeHasWeight);
}
