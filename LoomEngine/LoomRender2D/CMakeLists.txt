project(LoomRender2D LANGUAGES CXX)

# --- Renderer2D --- #
set(SOURCES
        Source/Render2D.cpp
)

set(HEADERS
        Include/Render2D.h
)

set(LoomRender2D_SOURCES
        ${SOURCES}
        ${HEADERS}
)

add_library(LoomRender2D SHARED ${LoomRender2D_SOURCES})

target_include_directories(LoomRender2D
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../LoomCore/Include
)


target_compile_definitions(LoomRender2D PUBLIC ENABLE_LoomRender2D)

# Private Definitions for this project
target_compile_definitions(LoomRender2D PRIVATE LOOM_BUILD_DLL)