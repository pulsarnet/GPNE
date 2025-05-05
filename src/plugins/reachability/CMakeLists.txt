set(SOURCES
        BuildReachabilityGraphTask.cpp
        BuildReachabilityGraphTask.h
        ReachabilityDocument.cpp
        ReachabilityDocument.h
        ReachabilityGraphScene.cpp
        ReachabilityGraphScene.h
        ReachabilityLine.cpp
        ReachabilityLine.h
        ReachabilityNode.cpp
        ReachabilityNode.h
        ReachabilityPlugin.cpp
        ReachabilityPlugin.h
        ReachabilityTreeEditor.cpp
        ReachabilityTreeEditor.h
        ReachabilityView.cpp
        ReachabilityView.h
        ReachabilityWindow.cpp
        ReachabilityWindow.h
        Constants.h
)

qt_add_plugin(
        ${TARGET_NAME}_reachability_plugin STATIC
        ${SOURCES}
        CLASS_NAME ReachabilityPlugin
        PLUGIN_TYPE "org.gpne.plugin.reachability"
)
target_link_libraries(${TARGET_NAME}_reachability_plugin PUBLIC ${TARGET_NAME}_editor_plugin ptn)
target_include_directories(${TARGET_NAME}_reachability_plugin PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)