set(SOURCES
        SimulationPlugin.cpp
        SimulationPlugin.h
        SimulationWidget.cpp
        SimulationWidget.h
)

qt_add_plugin(
        ${TARGET_NAME}_simulation_plugin STATIC
        ${SOURCES}
        CLASS_NAME SimulationPlugin
        PLUGIN_TYPE "org.gpne.plugin.simulation"
)
target_link_libraries(${TARGET_NAME}_simulation_plugin PUBLIC ${TARGET_NAME}_editor_plugin)
target_include_directories(${TARGET_NAME}_simulation_plugin PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)