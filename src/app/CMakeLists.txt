set(SOURCES main.cpp)
set(LINK_LIBRARIES ${TARGET_NAME}_lib)
if (USE_CRASHPAD)
        list(APPEND SOURCES crashpad.cpp crashpad.h)
        list(APPEND LINK_LIBRARIES crashpad::crashpad)
endif()

qt_add_executable(${TARGET_NAME} ${SOURCES})
add_dependencies(${TARGET_NAME} ${LINK_LIBRARIES})

set_target_properties(
        ${TARGET_NAME}
        PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        WIN32_EXECUTABLE  ON
        MACOSX_BUNDLE ON
)

target_link_libraries(${TARGET_NAME} PRIVATE ${LINK_LIBRARIES})

if(MSVC)
        set_target_properties(${TARGET_NAME} PROPERTIES
                LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
                LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        )
endif()