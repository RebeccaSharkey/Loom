project(Stitch)

# ----------- Setup Source Files  ----------- #
set(STITCH_SOURCES
        src/main.cpp
)
# ----------- Create Executable ----------- #
add_executable(Stitch ${STITCH_SOURCES})

# Link against the Loom engine
target_link_libraries(Stitch PRIVATE LoomEngine)

# ----------- Setup Definitions  ----------- #
if (WIN32)
    target_compile_definitions(Stitch PRIVATE LOOM_PLATFORM_WINDOWS)
endif()
