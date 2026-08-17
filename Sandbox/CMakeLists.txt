set(LOOM_SANDBOX_SOURCES
        Source/SandboxApp.cpp
)

add_executable(LoomSandbox ${LOOM_SANDBOX_SOURCES})

# Linked through the namespaced aliases, which is exactly how a generated game
# project will consume the installed engine. Keeps Sandbox an honest example.
target_link_libraries(LoomSandbox PRIVATE
        Loom::Core
        Loom::Render2D
)
