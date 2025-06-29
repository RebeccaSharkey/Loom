project(LoomCore LANGUAGES CXX)

# ----------- Core Module ----------- #
set(LOOM_CORE_SOURCES
        Source/Core/Application.cpp
        Source/Core/Assert.cpp
)

set(LOOM_CORE_HEADERS
        Include/Core/Core.h
        Include/Core/Application.h
        Include/Core/Assert.h
)

# ----------- Log Module ----------- #
set(LOOM_LOG_SOURCES
        Source/Log/Log.cpp
        Source/Log/LogStack.cpp
        Source/Log/Sinks/ILogSink.cpp
        Source/Log/Sinks/ConsoleSink.cpp
        Source/Log/Sinks/FileSink.cpp
)

set(LOOM_LOG_HEADERS
        Include/Log/LogMessage.h
        Include/Log/LogLevels.h
        Include/Log/Log.h
        Include/Log/LogStack.h
        Include/Log/Sinks/ILogSink.h
        Include/Log/Sinks/ConsoleSink.h
        Include/Log/Sinks/FileSink.h
)

# ----------- Event Module ----------- #
set(LOOM_EVENT_SOURCES
        Source/Events/EventDispatcher.cpp
)

set(LOOM_EVENT_HEADERS
        Include/Events/IEvent.h
        Include/Events/EventType.h
        Include/Events/EventDispatcher.h
        Include/Events/EventHandle.h
        Include/Events/EventMacro.h
)

# ----------- Minimum Required ----------- #
set(LOOM_MINIMUM_SOURCES

)

SET(LOOM_MINIMUM_HEADERS
        Include/EntryPoint.h
)

# ----------- MASTER SOURCE LIST ----------- #

set(LOOM_ENGINE_SOURCES
        ${LOOM_CORE_SOURCES}
        ${LOOM_CORE_HEADERS}
        ${LOOM_LOG_SOURCES}
        ${LOOM_LOG_HEADERS}
        ${LOOM_EVENT_SOURCES}
        ${LOOM_EVENT_HEADERS}
        ${LOOM_MINIMUM_SOURCES}
        ${LOOM_MINIMUM_HEADERS}
)

# ----------- Create Shared Library ----------- #
add_library(LoomCore SHARED ${LOOM_ENGINE_SOURCES})

# Public headers exposed to other projects
target_include_directories(LoomCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)

# Public definitions exposed to other projects
target_compile_definitions(LoomCore PRIVATE LOOM_BUILD_DLL)

target_compile_definitions(LoomCore PRIVATE $<$<CONFIG:Debug>:LOOM_DEBUG>)