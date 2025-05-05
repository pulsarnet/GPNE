set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Test)
qt_add_executable(actionmanager_test actionmanager.cpp)
add_test(NAME actionmanager_test COMMAND actionmanager_test)
target_link_libraries(actionmanager_test PRIVATE ${TARGET_NAME}_corelib Qt::Test)

qt_add_executable(projectutil_test projectutil.cpp)
add_test(NAME projectutil_test COMMAND projectutil_test)
target_link_libraries(projectutil_test PRIVATE ${TARGET_NAME}_corelib Qt::Test)