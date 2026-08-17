# ----------- Render2D Module ----------- #
set(LOOM_RENDER2D_SOURCES
        Source/Render2D.cpp
)

set(LOOM_RENDER2D_HEADERS
        Include/Render2D.h
)

# ----------- Create Static Library ----------- #
add_library(LoomRender2D STATIC
        ${LOOM_RENDER2D_SOURCES}
        ${LOOM_RENDER2D_HEADERS}
)
add_library(Loom::Render2D ALIAS LoomRender2D)

# PUBLIC: Render2D's headers expose LoomCore types, so consumers need it too.
target_link_libraries(LoomRender2D PUBLIC LoomCore)

# Public headers exposed to other projects
target_include_directories(LoomRender2D PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
