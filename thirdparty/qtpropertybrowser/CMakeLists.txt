cmake_minimum_required(VERSION 3.21)
project(qtpropertybrowser LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Core5Compat)

set(SOURCE_FILES
        src/qtpropertybrowser.cpp
        src/qtpropertymanager.cpp
        src/qteditorfactory.cpp
        src/qtvariantproperty.cpp
        src/qttreepropertybrowser.cpp
        src/qtbuttonpropertybrowser.cpp
        src/qtgroupboxpropertybrowser.cpp
        src/qtpropertybrowserutils.cpp
)
set(HEADER_FILES
        src/qtpropertybrowser.h
        src/qtpropertymanager.h
        src/qteditorfactory.h
        src/qtvariantproperty.h
        src/qttreepropertybrowser.h
        src/qtbuttonpropertybrowser.h
        src/qtgroupboxpropertybrowser.h
        src/qtpropertybrowserutils.h
)
set(QRC_FILES src/qtpropertybrowser.qrc)
include_directories(src)
include_directories(${QT_INCLUDE_DIR})

qt_add_resources(QT_RESOURCES ${QRC_FILES})
qt_add_library(qtpropertybrowser STATIC
        ${SOURCE_FILES}
        ${QT_RESOURCES}
)

target_include_directories(qtpropertybrowser PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(qtpropertybrowser PRIVATE
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Core5Compat
)
set_target_properties(qtpropertybrowser PROPERTIES AUTOMOC TRUE)
set_target_properties(qtpropertybrowser PROPERTIES AUTORCC TRUE)
