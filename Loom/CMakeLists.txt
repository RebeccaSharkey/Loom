cmake_minimum_required(VERSION 3.20)
project(Loom LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------- Core Module ----------- #
set(LOOM_CORE_SOURCES
        src/LoomEngine/Core/Application.cpp
)

set(LOOM_CORE_HEADERS
        include/Loom/LoomEngine/EntryPoint.h
        include/Loom/LoomEngine/Core/Core.h
        include/Loom/LoomEngine/Core/Application.h
)

# ----------- Minimum Required ----------- #
set(LOOM_MINIMUM_SOURCES
        src/LoomEngine/Log/Log.cpp
)

SET(LOOM_MINIMUM_HEADERS
        include/Loom/LoomEngine/Log/LogLevels.h
        include/Loom/LoomEngine/Log/Log.h
)

# ----------- MASTER SOURCE LIST ----------- #

set(LOOM_ENGINE_SOURCES
        ${LOOM_MINIMUM_SOURCES}
        ${LOOM_MINIMUM_HEADERS}
        ${LOOM_CORE_SOURCES}
        ${LOOM_CORE_HEADERS}
)

# ----------- Create Shared Library ----------- #
add_library(LoomEngine SHARED ${LOOM_ENGINE_SOURCES})

# Public headers exposed to other projects
target_include_directories(LoomEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/Loom/LoomEngine)

# Public definitions exposed to other projects
target_compile_definitions(LoomEngine PRIVATE LOOM_BUILD_DLL)

if (WIN32)
    target_compile_definitions(LoomEngine PRIVATE LOOM_PLATFORM_WINDOWS)
endif()

# ----------- OPTIONAL MODULES ----------- #

option(LOOM_ENABLE_RENDERER2D "Enable Renderer2D" ON)

# --- Renderer2D --- #
if(LOOM_ENABLE_RENDERER2D)
    set(LOOM_RENDERER2D_SOURCES
            src/Renderer2D/Renderer2D.cpp
    )

    set(LOOM_RENDERER2D_HEADERS
            include/Loom/Renderer2D/Renderer2D.h
    )

    set(LOOM_RENDER2D_SOURCES
            ${LOOM_RENDERER2D_SOURCES}
            ${LOOM_RENDERER2D_HEADERS}
    )

    add_library(LoomRender2D SHARED ${LOOM_RENDER2D_SOURCES})

    target_include_directories(LoomRender2D
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/Loom/Renderer2D
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/Loom/LoomEngine
    )

    target_compile_definitions(LoomRender2D PUBLIC LOOM_ENABLE_RENDERER2D)
endif()


