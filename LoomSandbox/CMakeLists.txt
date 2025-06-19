project(LoomSandbox)

add_executable(LoomSandbox
    Source/main.cpp)

if (WIN32)
    target_compile_definitions(LoomSandbox PRIVATE LOOM_PLATFORM_WINDOWS)
endif()

target_link_libraries(LoomSandbox
        PRIVATE
        LoomCore
        LoomRender2D
)