# Add Qt-Advanced-Docking-System
#set(BUILD_STATIC FALSE CACHE BOOL "ADS: Build shared library")
set(BUILD_EXAMPLES OFF CACHE BOOL "ADS: Build examples")
set(ADS_VERSION "4.4.0" CACHE STRING "ADS: Version")
add_subdirectory(qtads EXCLUDE_FROM_ALL)
set_target_properties(qtadvanceddocking-qt6 PROPERTIES PROJECT_LABEL "qtadvanceddocking" OUTPUT_NAME "qtadvanceddocking")


# Add qtpropertybrowser
add_subdirectory(qtpropertybrowser EXCLUDE_FROM_ALL)
