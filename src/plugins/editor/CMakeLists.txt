set(SOURCES
        elements/PetriObject.cpp
        elements/PetriObject.h
        model/JsonFormat.cpp
        model/JsonFormat.h
        model/NetModel.cpp
        model/NetModel.h
        registry/DirectedEdge.cpp
        registry/DirectedEdge.h
        registry/IItem.cpp
        registry/IItem.h
        registry/IEdge.cpp
        registry/IEdge.h
        registry/InhibitorEdge.cpp
        registry/InhibitorEdge.h
        registry/IVertex.cpp
        registry/IVertex.h
        registry/Place.cpp
        registry/Place.h
        registry/Registry.cpp
        registry/Registry.h
        registry/Transition.cpp
        registry/Transition.h
        toolbox/Tool.cpp
        toolbox/Tool.h
        toolbox/ToolBox.cpp
        toolbox/ToolBox.h
        topology/TopologyModel.cpp
        topology/TopologyModel.h
        topology/TopologyWidget.cpp
        topology/TopologyWidget.h
        EditorPlugin.cpp
        EditorPlugin.h
        EditorWidget.cpp
        EditorWidget.h
        GraphicsScene.cpp
        GraphicsScene.h
        GraphicsSceneActions.cpp
        GraphicsSceneActions.h
        GraphicsView.cpp
        GraphicsView.h
        GraphicsViewZoom.cpp
        GraphicsViewZoom.h
        PetriNetDocument.cpp
        PetriNetDocument.h
        PetriNetEditor.cpp
        PetriNetEditor.h
        Macros.h
        Constants.h
        EditorToolController.cpp
        EditorToolController.h
)

qt_add_plugin(
        ${TARGET_NAME}_editor_plugin STATIC
        ${SOURCES}
        CLASS_NAME EditorPlugin
        PLUGIN_TYPE "org.gpne.plugin.editor"
)

target_link_libraries(${TARGET_NAME}_editor_plugin PUBLIC ${TARGET_NAME}_corelib ptn)
target_include_directories(${TARGET_NAME}_editor_plugin PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)
