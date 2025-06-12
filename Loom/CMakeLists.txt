project(LoomEngine)

# ----------- Core Module ----------- #
set(LOOM_CORE_SOURCES
        src/Core/Application.cpp
)

set(LOOM_CORE_HEADERS
        include/Loom/Core/Core.h
        include/Loom/Core/Application.h
)

# Add to master source list
set(LOOM_ENGINE_SOURCES
        ${LOOM_CORE_SOURCES}
        ${LOOM_CORE_HEADERS}
)

# ----------- Create Shared Library ----------- #
add_library(LoomEngine SHARED ${LOOM_ENGINE_SOURCES})

# Public headers exposed to other projects
target_include_directories(LoomEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ----------- Setup Definitions  ----------- #
target_compile_definitions(LoomEngine PRIVATE LOOM_BUILD_DLL)

if (WIN32)
    target_compile_definitions(LoomEngine PRIVATE LOOM_PLATFORM_WINDOWS)
endif()