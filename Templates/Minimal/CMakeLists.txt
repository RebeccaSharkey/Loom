cmake_minimum_required(VERSION 3.25)
project(__LOOM_NAME__ LANGUAGES CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported configurations" FORCE)

# An explicit project selection wins; never fall back from an invalid selection.
if(DEFINED LOOM_ENGINE_ROOT)
    set(_loom_root "${LOOM_ENGINE_ROOT}")
else()
    set(_loom_root "$ENV{LOOM_ROOT}")
endif()
if(NOT EXISTS "${_loom_root}/lib/cmake/Loom/LoomProject.cmake")
    message(FATAL_ERROR "Loom: invalid engine selection '${_loom_root}'. Set LOOM_ENGINE_ROOT in CMakeUserPresets.json or set LOOM_ROOT.")
endif()
get_filename_component(_loom_root "${_loom_root}" ABSOLUTE)
include("${_loom_root}/lib/cmake/Loom/LoomProject.cmake")
loom_read_project("${CMAKE_CURRENT_SOURCE_DIR}/__LOOM_NAME__.loomproject" LOOM_MODULES)
if(NOT LOOM_PROJECT_Name STREQUAL PROJECT_NAME)
    message(FATAL_ERROR "Loom: descriptor Name must match the project/target identifier '${PROJECT_NAME}'. Rename the target and descriptor file together.")
endif()
# Discard a previously cached package choice when changing engine installation.
set(Loom_DIR "${_loom_root}/lib/cmake/Loom" CACHE PATH "Selected Loom package" FORCE)
find_package(Loom ${LOOM_PROJECT_EngineVersion} CONFIG REQUIRED
    COMPONENTS ${LOOM_MODULES} PATHS "${Loom_DIR}" NO_DEFAULT_PATH)
message(STATUS "Loom: selected ${Loom_DIR} (required ${LOOM_PROJECT_EngineVersion})")

add_executable(__LOOM_NAME__ Source/main.cpp)
loom_configure_target(__LOOM_NAME__)
