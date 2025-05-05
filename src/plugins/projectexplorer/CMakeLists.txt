set(SOURCES
        ProjectExplorerPlugin.cpp
        ProjectExplorerPlugin.h
        ProjectExplorerModel.h
        ProjectExplorerModel.cpp
        ProjectExplorerView.h
        ProjectExplorerView.cpp
        ProjectFileIndexer.cpp
        ProjectFileIndexer.h
        NewNetDialog.cpp
        NewNetDialog.h
        Constants.h
)

find_package(Boost CONFIG REQUIRED COMPONENTS multi_index callable_traits)
qt_add_plugin(
        ${TARGET_NAME}_projectexplorer_plugin STATIC
        ${SOURCES}
        CLASS_NAME ProjectExplorerPlugin
        PLUGIN_TYPE "org.gpne.plugin.projectexplorer"
)
target_link_libraries(${TARGET_NAME}_projectexplorer_plugin PUBLIC ${TARGET_NAME}_editor_plugin Boost::multi_index Boost::callable_traits)
target_include_directories(${TARGET_NAME}_projectexplorer_plugin PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)