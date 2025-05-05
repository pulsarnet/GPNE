add_executable(ptn_tests
        net.cpp
        vec.cpp
        matrix.cpp
)

target_link_libraries(ptn_tests PRIVATE ptn Catch2::Catch2WithMain)
if (APPLE)
    target_link_libraries(ptn_tests PRIVATE "-framework Foundation")
endif ()

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_test(
        NAME ptn_rust_test
        COMMAND ${CMAKE_COMMAND} -E env CARGO_BUILD_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR} cargo test #TODO: maybe use CMAKE_BINARY_DIR
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

set(PARSE_CATCH_TESTS_VERBOSE ON)
catch_discover_tests(ptn_tests REPORTER "console::out=-")
