cmake_minimum_required(VERSION 3.20)
project(LoomEngine LANGUAGES CXX)

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
        src/LoomEngine/Events/Event.cpp
)

SET(LOOM_MINIMUM_HEADERS
        include/Loom/LoomEngine/Log/LogLevels.h
        include/Loom/LoomEngine/Log/Log.h
        include/Loom/LoomEngine/Events/Event.h
)

# ----------- MASTER SOURCE LIST ----------- #

set(LOOM_ENGINE_SOURCES
        ${LOOM_CORE_SOURCES}
        ${LOOM_CORE_HEADERS}
        ${LOOM_MINIMUM_SOURCES}
        ${LOOM_MINIMUM_HEADERS}
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

option(LOOM_ENABLE_RENDERER2D "Enable Renderer2D" OFF)

# --- Renderer2D --- #
if(LOOM_ENABLE_RENDERER2D)
    set(LOOM_RENDERER2D_SOURCES
            src/LoomRender2D/Render2D.cpp
    )

    set(LOOM_RENDERER2D_HEADERS
            include/Loom/LoomRender2D/Render2D.h
    )

    set(LOOM_RENDER2D_SOURCES
            ${LOOM_RENDERER2D_SOURCES}
            ${LOOM_RENDERER2D_HEADERS}
    )

    add_library(LoomRender2D SHARED ${LOOM_RENDER2D_SOURCES})

    target_include_directories(LoomRender2D
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/Loom/LoomRender2D
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/Loom/LoomEngine
    )

    target_compile_definitions(LoomRender2D PUBLIC LOOM_ENABLE_RENDERER2D)
endif()


