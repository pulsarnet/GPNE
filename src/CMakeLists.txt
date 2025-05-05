add_subdirectory(app)
add_subdirectory(libs)
add_subdirectory(plugins)

qt_add_library(${TARGET_NAME}_lib STATIC ${WIN_RC_FILE})
qt_import_plugins(${TARGET_NAME}_lib INCLUDE Qt::Svg NO_DEFAULT)

target_link_libraries(${TARGET_NAME}_lib PUBLIC
        ${TARGET_NAME}_corelib
        ${TARGET_NAME}_editor_plugin
        ${TARGET_NAME}_simulation_plugin
        ${TARGET_NAME}_projectexplorer_plugin
        ${TARGET_NAME}_iomatrix_plugin
        ${TARGET_NAME}_reachability_plugin
        ${TARGET_NAME}_synthesis_plugin
)

set_target_properties(${TARGET_NAME}_lib PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        CXX_STANDART 23
        CXX_STANDART_REQUIRED ON
        CXX_EXTENSIONS OFF
        #COMPILE_WARNING_AS_ERROR ON
)
target_include_directories(${TARGET_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
