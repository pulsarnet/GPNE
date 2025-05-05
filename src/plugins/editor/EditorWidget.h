// SPDX-FileCopyrightText: 2025 Nickolay Muravev
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 Nickolay Muravev

#ifndef GPNE_NETMODELINGTAB_H
#define GPNE_NETMODELINGTAB_H

#include <DockManager.h>

class QtBrowserItem;
class QtIntPropertyManager;
class QtEnumPropertyManager;
struct ObjectPropertyInfo;
class QtVariantPropertyManager;
class QtTreePropertyBrowser;
class IEdge;
class IVertex;
class QtProperty;
class GraphicsView;
class NetModel;

class EditorWidget : public QWidget {

    Q_OBJECT

public:
    explicit EditorWidget(NetModel* model, QWidget* parent = nullptr);

    [[nodiscard]]
    GraphicsView* view() const;

public slots:

    void onVertexPropertyChanged(QtProperty*, const QVariant& value);
    void onEdgePropertyChanged(QtProperty*, const QVariant& value);

    void onModelSelectionChanged();
    void onVertexChanged(IVertex*);
    void onEdgeChanged(IEdge*);

    void onMatrixWindowClose(QWidget* window);

protected:
    void initGraphicsView();
    void initTopologyView();
    void initPropertyEditor();

    void updateVisibility();

private:
    NetModel* m_model{nullptr};

    ads::CDockManager* m_dockManager{nullptr};
    ads::CDockWidget* m_view{nullptr};
    ads::CDockWidget* m_topologyView{nullptr};
    ads::CDockWidget* m_propertyView{nullptr};

    QtTreePropertyBrowser* m_propertyBrowser{nullptr};
    QtVariantPropertyManager* m_vertexPropertyManager{nullptr};
    QHash<QByteArray, ObjectPropertyInfo*> m_vertexPropertyGroups;

    // edge property editing
    QtVariantPropertyManager* m_edgePropertyManager{nullptr};
    QtEnumPropertyManager* m_directionPropertyManager{nullptr};
    QtIntPropertyManager* m_intPropertyManager{nullptr};

    QtBrowserItem* m_directionChooser;
    QtBrowserItem* m_weightPT;
    QtBrowserItem* m_weightTP;

    IVertex* m_currentVertex{nullptr};
    IEdge* m_currentEdge{nullptr};
};

#endif // GPNE_NETMODELINGTAB_H
