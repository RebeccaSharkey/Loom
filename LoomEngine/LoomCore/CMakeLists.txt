project(LoomCore LANGUAGES CXX)

# ----------- Core Module ----------- #
set(LOOM_CORE_SOURCES
        Source/Core/Application.cpp
)

set(LOOM_CORE_HEADERS
        Include/EntryPoint.h
        Include/Core/Core.h
        Include/Core/Application.h
)

# ----------- Minimum Required ----------- #
set(LOOM_MINIMUM_SOURCES
        Source/Log/Log.cpp
        Source/Log/LogStack.cpp
        Source/Log/Sinks/ILogSink.cpp
        Source/Log/Sinks/ConsoleSink.cpp
        Source/Log/Sinks/FileSink.cpp
        Source/Events/Event.cpp
)

SET(LOOM_MINIMUM_HEADERS
        Include/Log/LogMessage.h
        Include/Log/LogLevels.h
        Include/Log/Log.h
        Include/Log/LogStack.h
        Include/Log/Sinks/ILogSink.h
        Include/Log/Sinks/ConsoleSink.h
        Include/Log/Sinks/FileSink.h
        Include/Events/Event.h
)

# ----------- MASTER SOURCE LIST ----------- #

set(LOOM_ENGINE_SOURCES
        ${LOOM_CORE_SOURCES}
        ${LOOM_CORE_HEADERS}
        ${LOOM_MINIMUM_SOURCES}
        ${LOOM_MINIMUM_HEADERS}
)

# ----------- Create Shared Library ----------- #
add_library(LoomCore SHARED ${LOOM_ENGINE_SOURCES})

# Public headers exposed to other projects
target_include_directories(LoomCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

# Public definitions exposed to other projects
target_compile_definitions(LoomCore PRIVATE LOOM_BUILD_DLL)

if (WIN32)
    target_compile_definitions(LoomCore PRIVATE LOOM_PLATFORM_WINDOWS)
endif()