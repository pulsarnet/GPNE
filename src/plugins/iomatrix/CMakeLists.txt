set(SOURCES
        MatrixPlugin.cpp
        MatrixPlugin.h
        MatrixWindow.cpp
        MatrixWindow.h
        NamedMatrixModel.h
        Constants.h
)

qt_add_plugin(
        ${TARGET_NAME}_iomatrix_plugin STATIC
        ${SOURCES}
        CLASS_NAME MatrixPlugin
        PLUGIN_TYPE "org.gpne.plugin.iomatrix"
)
target_link_libraries(${TARGET_NAME}_iomatrix_plugin PUBLIC ${TARGET_NAME}_editor_plugin ptn)
target_include_directories(${TARGET_NAME}_iomatrix_plugin PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/..>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)