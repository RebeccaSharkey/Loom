cmake_minimum_required(VERSION 3.25)
project(Loom LANGUAGES CXX VERSION 0.1.0 DESCRIPTION "Loom Engine")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Multi-config generators append the configuration, e.g. Build/Bin/Debug.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/Lib)

set(CMAKE_DEBUG_POSTFIX "d")

add_subdirectory(LoomEngine)
add_subdirectory(Sandbox)

# Make Visual Studio launch the sandbox rather than ALL_BUILD.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LoomSandbox)

### INSTALLING ###
# Install the libraries
install(TARGETS LoomCore       EXPORT LoomCoreTargets)
install(TARGETS LoomRender2D   EXPORT LoomRender2DTargets)

# Install the headers
install(DIRECTORY LoomEngine/LoomCore/Public/          DESTINATION include/LoomCore FILES_MATCHING PATTERN "*.h")
install(DIRECTORY LoomEngine/LoomRender2D/Include/      DESTINATION include/LoomRender2D FILES_MATCHING PATTERN "*.h")

# Creates the package
install(EXPORT LoomCoreTargets     NAMESPACE Loom:: DESTINATION lib/cmake/Loom)
install(EXPORT LoomRender2DTargets NAMESPACE Loom:: DESTINATION lib/cmake/Loom)

# Generate Looms config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
        CMake/LoomConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/LoomConfig.cmake
        INSTALL_DESTINATION lib/cmake/Loom
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/LoomConfig.cmake
        DESTINATION lib/cmake/Loom
)

install(FILES
        CMake/LoomProject.cmake
        DESTINATION lib/cmake/Loom
)

# Bundle third party dependencies with the engine install
install(DIRECTORY ${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/
        DESTINATION third-party
)