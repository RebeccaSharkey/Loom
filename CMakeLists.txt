cmake_minimum_required(VERSION 3.25)
project(Loom LANGUAGES CXX)

# The C++ standard is set on LoomCore as a PUBLIC compile feature, not here.
# A global only affects this build tree; a target property travels with the
# target when a game consumes the installed engine.

# Pin the CRT so static third-party libs can't disagree with us later.
# Dynamic CRT (/MD, /MDd) pairs with the x64-windows-static-md vcpkg triplet.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Multi-config generators append the configuration, e.g. Build/Bin/Debug.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/Lib)

add_subdirectory(LoomEngine)
add_subdirectory(Sandbox)

# Make Visual Studio launch the sandbox rather than ALL_BUILD.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LoomSandbox)

