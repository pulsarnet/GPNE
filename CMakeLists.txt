cmake_minimum_required(VERSION 3.30)
project(GPNE VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

message(STATUS $ENV{PATH})
set(TARGET_NAME GPNE)
set(TARGET_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR}/archive)
set(CMAKE_AUTORCC ON)
set(VERSION_COMPONENT_BUILD 1)
set(CMAKE_VERBOSE_MAKEFILE on)

if(WIN32)
    set(CMAKE_INSTALL_BINDIR ".")
    set(CMAKE_INSTALL_LIBDIR ".")
    set(CMAKE_INSTALL_INCLUDEDIR "include")
    set(CMAKE_INSTALL_DATADIR ".")
    set(CMAKE_INSTALL_DOCDIR ".")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(COMPONENTS Widgets Core Gui Svg DataVisualization LinguistTools Core5Compat Concurrent)
find_package(Qt6 REQUIRED COMPONENTS ${COMPONENTS})
find_package(Boost CONFIG REQUIRED)
find_package(reflectcpp CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

if(USE_CRASHPAD)
    find_package(Crashpad CONFIG REQUIRED)
    find_program(CRASHPAD_HANDLER_EXECUTABLE crashpad_handler${CMAKE_EXECUTABLE_SUFFIX} HINTS "${CMAKE_PROGRAM_PATH}" REQUIRED)
    add_definitions(-DUSE_CRASHPAD)
endif()

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

qt6_standard_project_setup()
get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_path ${QT_QMAKE_EXECUTABLE} DIRECTORY)
set(QT_BIN_PATH "${_qt_bin_path}" CACHE INTERNAL "Path to qmake, macdeployqt, windeployqt, and other Qt utilities.")
message(STATUS "QT_BIN_PATH: ${QT_BIN_PATH}")

if(WIN32)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gpne.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/gpne.rc)
    set(WIN_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/gpne.rc)
else()
    add_compile_definitions(__cpp_lib_format=201907L)
endif()

add_subdirectory(thirdparty)
add_subdirectory(rust/ptn-sys)
add_subdirectory(src)

# Fix build Qt>=6.9.0
if (Qt6Core_VERSION VERSION_GREATER_EQUAL 6.9.0 AND UNIX AND NOT APPLE)
    target_link_libraries(qtadvanceddocking-qt6 PRIVATE Qt6::GuiPrivate)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${QT_BIN_PATH};$ENV{PATH}")

if (WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
    include(InstallRequiredSystemLibraries)

    set(ADDITIONAL_WINDEPLOYQT_OPTIONS)
    if (BUILD_TESTING)
        list(APPEND ADDITIONAL_WINDEPLOYQT_OPTIONS --test)
    endif()

    find_package(PowerShell REQUIRED)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/windeployqt)
    find_program(QT_WINDEPLOYQT_EXECUTABLE "windeployqt" HINTS ${QT_BIN_PATH} REQUIRED)
    add_custom_target(
            windeploy_qt_target ALL
            COMMAND ${CMAKE_COMMAND} -E rm -rRf "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
            COMMAND set "PATH=${QT_BIN_PATH};%PATH%"
            COMMAND "${QT_WINDEPLOYQT_EXECUTABLE}"
            --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
            --no-compiler-runtime
            --no-opengl-sw
            --no-translations
            --no-system-d3d-compiler
            --no-quick-import
            --no-network
            ${ADDITIONAL_WINDEPLOYQT_OPTIONS}
            --skip-plugin-types networkinformation,tls,iconengines,generic
            --verbose 3
            "$<TARGET_FILE:${TARGET_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            VERBATIM
            DEPENDS ${TARGET_NAME}
    )


    add_custom_target(qtads_deploy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:qtadvanceddocking-qt6> $<TARGET_FILE_DIR:${TARGET_NAME}>
            DEPENDS windeploy_qt_target
    )

    if (USE_CRASHPAD)
        add_custom_target(
                copy_crashpad_handler ALL
                WORKING_DIRECTORY $<TARGET_FILE_DIR:${TARGET_NAME}>
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CRASHPAD_HANDLER_EXECUTABLE} $<TARGET_FILE_DIR:${TARGET_NAME}>
                USES_TERMINAL
                VERBATIM
                DEPENDS windeploy_qt_target
        )
    endif()

    qt_generate_deploy_app_script(
            TARGET ${TARGET_NAME}
            OUTPUT_SCRIPT  GPNE_deploy_script
            NO_UNSUPPORTED_PLATFORM_ERROR
            DEPLOY_TOOL_OPTIONS --no-system-d3d-compiler --no-compiler-runtime --no-quick-import --skip-plugin-types networkinformation,tls,iconengines,generic
    )

    install(SCRIPT ${GPNE_deploy_script})

elseif(APPLE)
    set_target_properties(
            ${TARGET_NAME}
            PROPERTIES
            BUNDLE TRUE
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
            MACOSX_BUNDLE_GUI_IDENTIFIER com.${PROJECT_NAME}.gui
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
    )

    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist.in
            ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
            @ONLY
    )

    if (GENERATE_BUNDLE)
        add_compile_definitions(MACOS_BUNDLE)
        find_program(QT_MACDEPLOYQT_EXECUTABLE macdeployqt
                HINTS "${QT_BIN_PATH}"
                DOC "Path to the macdeployqt utility."
                REQUIRED
        )
        add_custom_target(
                mac_deploy_qt
                ALL
                DEPENDS ${TARGET_NAME}
                COMMAND "${QT_MACDEPLOYQT_EXECUTABLE}" $<TARGET_BUNDLE_DIR:${TARGET_NAME}> -libpath=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -always-overwrite -codesign=- -verbose=3
        )

    endif ()

    install(TARGETS ${TARGET_NAME} BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# -----------------------------------------------------------------------------
# Translation
# -----------------------------------------------------------------------------
qt_add_translations(${TARGET_NAME} TS_FILES translation/gpne_en.ts translation/gpne_ru.ts)

# -----------------------------------------------------------------------------
# Packaging (CPack)
# -----------------------------------------------------------------------------
include(ConfigCPack.cmake)

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
