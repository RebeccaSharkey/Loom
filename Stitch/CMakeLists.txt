project(Stitch)

# Declare this subproject
project(Stitch)

# Executable source files
set(STITCH_SOURCES
        src/main.cpp
)

# Create the executable
add_executable(Stitch ${STITCH_SOURCES})

# Link against the Loom engine
target_link_libraries(Stitch PRIVATE LoomEngine)