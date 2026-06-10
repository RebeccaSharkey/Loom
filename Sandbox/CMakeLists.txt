project(LoomSandbox LANGUAGES CXX)

set(LOOM_SANDBOX_SOURCES
        Source/SandboxApp.cpp
)

add_executable(LoomSandbox ${LOOM_SANDBOX_SOURCES})

target_link_libraries(LoomSandbox PRIVATE LoomCore)
target_link_libraries(LoomSandbox PRIVATE LoomRender2D)

target_compile_definitions(LoomSandbox PRIVATE $<$<CONFIG:Debug>:LOOM_DEBUG>)

if(WIN32)
    target_compile_definitions(LoomSandbox PRIVATE LOOM_PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(LoomSandbox PRIVATE LOOM_PLATFORM_MACOS)
elseif(UNIX)
    target_compile_definitions(LoomSandbox PRIVATE LOOM_PLATFORM_LINUX)
endif()
