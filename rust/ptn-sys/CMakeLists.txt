find_package(Corrosion CONFIG REQUIRED)

set(CRATE_TYPES "staticlib")
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND MSVC)
    set(CRATE_TYPES "cdylib")
endif()

# Import targets defined in a package or workspace manifest `Cargo.toml` file
corrosion_import_crate(
        MANIFEST_PATH Cargo.toml
        CRATE_TYPES  ${CRATE_TYPES}
)

set(PTN_SYS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cxx/include")
set(
        PTN_SYS_SOURCES
        cxx/src/decompose.cpp
        cxx/src/edge.cpp
        cxx/src/logger.cpp
        cxx/src/matrix.cpp
        cxx/src/net.cpp
        cxx/src/place.cpp
        cxx/src/reachability.cpp
        cxx/src/simulation.cpp
        cxx/src/vec.cpp
        cxx/src/slice.cpp
)

add_library(ptn STATIC ${PTN_SYS_SOURCES})
target_include_directories(ptn PUBLIC ${PTN_SYS_INCLUDE_DIR})
target_link_libraries(ptn ptn_sys)

if (BUILD_TESTING)
    # tests
    enable_testing()
    add_subdirectory(tests)
endif ()

