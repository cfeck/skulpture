project (skulpture)
cmake_minimum_required (VERSION 2.6)

set (KDE_MIN_VERSION "4.4.0") # for the < 4.2 macro
find_package (KDE4 4.4.0 REQUIRED)
find_package (KDE4Workspace REQUIRED)

include (KDE4Defaults)
include (MacroLibrary)
add_definitions (
    ${QT_DEFINITIONS}
    ${KDE4_DEFINITIONS}
)
include_directories (
    ${KDE4_INCLUDES}
    ${KDE4WORKSPACE_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}
)

add_subdirectory (src)
add_subdirectory (color-schemes)
add_subdirectory (kwin-client)

