project(LoomEngine)

option(LOOM_ENABLE_RENDERER2D "Enable Renderer2D" ON)

# ----------- Minimum Required ----------- #
set(LOOM_MINIMUM_SOURCES
        src/Log/Log.cpp
)

SET(LOOM_MINIMUM_HEADERS
        include/Loom/EntryPoint.h
        include/Loom/Log/LogLevels.h
        include/Loom/Log/Log.h
)

# ----------- Core Module ----------- #
set(LOOM_CORE_SOURCES
        src/Core/Application.cpp
)

set(LOOM_CORE_HEADERS
        include/Loom/Core/Core.h
        include/Loom/Core/Application.h
)

# ----------- MASTER SOURCE LIST ----------- #

set(LOOM_ENGINE_SOURCES
        ${LOOM_MINIMUM_SOURCES}
        ${LOOM_MINIMUM_HEADERS}
        ${LOOM_CORE_SOURCES}
        ${LOOM_CORE_HEADERS}
)

# ----------- OPTIONAL MODULES ----------- #

# --- Renderer2D --- #
if(LOOM_ENABLE_RENDERER2D)
    set(LOOM_RENDERER2D_SOURCES
            src/Renderer2D/Renderer2D.cpp
    )

    set(LOOM_RENDERER2D_HEADERS
            include/Loom/Renderer2D/Renderer2D.h
    )

    list(APPEND LOOM_ENGINE_SOURCES
            ${LOOM_RENDERER2D_SOURCES}
            ${LOOM_RENDERER2D_HEADERS}
    )
endif()


# ----------- Create Shared Library ----------- #
add_library(LoomEngine SHARED ${LOOM_ENGINE_SOURCES})

# Public headers exposed to other projects
target_include_directories(LoomEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ----------- Setup Definitions  ----------- #
target_compile_definitions(LoomEngine PRIVATE LOOM_BUILD_DLL)

if (WIN32)
    target_compile_definitions(LoomEngine PRIVATE LOOM_PLATFORM_WINDOWS)
endif()

if(LOOM_ENABLE_RENDERER2D)
    target_compile_definitions(LoomEngine PUBLIC LOOM_ENABLE_RENDERER2D)
endif ()