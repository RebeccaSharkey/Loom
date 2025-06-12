project(LoomEngine)

# ----------- Core Module ----------- #
set(LOOM_CORE_SOURCES
        src/Core/Application.cpp
)

set(LOOM_CORE_HEADERS
        include/Loom/Core/Application.h
)

# Add to master source list
set(LOOM_ENGINE_SOURCES
        ${LOOM_CORE_SOURCES}
        ${LOOM_CORE_HEADERS}
)

# ----------- Optional Module Toggles ----------- #
# option(LOOM_ENABLE_RENDERER2D "Enable 2D Renderer" )
# option(LOOM_ENABLE_DIALOGUE_SYSTEM "Enable Dialogue System" OFF)

# ----------- Renderer2D Module (Optional) ----------- #
#[[if(LOOM_ENABLE_RENDERER2D)
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
endif()]]

# ----------- Create Static Library ----------- #
add_library(LoomEngine STATIC ${LOOM_ENGINE_SOURCES})

# Public headers exposed to other projects
target_include_directories(LoomEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)