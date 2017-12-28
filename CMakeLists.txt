cmake_minimum_required(VERSION 2.8)
project(colorchecker_calibration_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# ccache
# -----------------------------------------------------------------------------

# Use ccache for compiling if available on machine.
option(USE_CCACHE "Use ccache if available" ON)
if(USE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
    mark_as_advanced(CCACHE_FOUND)
endif(USE_CCACHE)

# -----------------------------------------------------------------------------
# Build folder structure
# -----------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(INSTALL_PATH "${PROJECT_SOURCE_DIR}/installdir")
set(EXECUTABLE_OUTPUT_PATH "${INSTALL_PATH}/bin")
set(LIBRARY_OUTPUT_PATH "${INSTALL_PATH}/lib")
set(CMAKE_INSTALL_PREFIX ${INSTALL_PATH})

set(THIRDPARTY_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/thirdparty/")

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs/")

include(cmake/ConfigureBoost.cmake)
include(cmake/ConfigureCeres.cmake)
include(cmake/ConfigureEigen.cmake)
include(cmake/ConfigureGFLAGS.cmake)
include(cmake/ConfigureGLFW.cmake)
include(cmake/ConfigureGLOG.cmake)
include(cmake/ConfigureGLUT.cmake)
include(cmake/ConfigureOpenCV.cmake)
include(cmake/ConfigureOpenGL.cmake)
include(cmake/ConfigureOpenMP.cmake)
include(cmake/ConfigureSuiteSparse.cmake)
if(UNIX AND NOT APPLE)
    include(cmake/ConfigureX11.cmake)
endif()

include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
include_directories(SYSTEM ${THIRDPARTY_INCLUDE_DIR})

# -----------------------------------------------------------------------------
# Platform specific switches that won't fit anywhere else
# -----------------------------------------------------------------------------

if(APPLE)
    include(cmake/AppleHacks.cmake)
endif()

# -----------------------------------------------------------------------------
# Configure testing
# -----------------------------------------------------------------------------

enable_testing()
option(BUILD_TESTS "Build tests" ON)
add_subdirectory(test)

# -----------------------------------------------------------------------------
# Subdirectories
# -----------------------------------------------------------------------------

add_subdirectory(apps/colorchecker_calibrator)
add_subdirectory(libs/color_calibration)
add_subdirectory(libs/common)
add_subdirectory(libs/file_io_toolbox)
add_subdirectory(libs/geometry_toolbox)
add_subdirectory(libs/image_toolbox)
