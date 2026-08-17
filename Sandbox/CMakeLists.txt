project(LoomSandbox LANGUAGES CXX)

set(LOOM_SANDBOX_SOURCES
        Source/SandboxApp.cpp
)

add_executable(LoomSandbox ${LOOM_SANDBOX_SOURCES})

target_link_libraries(LoomSandbox PRIVATE LoomCore)
target_link_libraries(LoomSandbox PRIVATE LoomRender2D)
