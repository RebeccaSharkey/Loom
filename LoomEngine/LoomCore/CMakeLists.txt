# ----------- Core Module ----------- #
set(LOOMCORE_CORE_SOURCES
        Public/Core/Core.h
        Public/Core/Application.h
        Public/Core/Assert.h
        Public/Core/Types.h
        Public/Core/Time.h

        Private/Core/Application.cpp
        Private/Core/Assert.cpp
        Private/Core/Time.cpp
)

# ----------- Math Module ----------- #
set(LOOMCORE_MATH_SOURCES
        Public/Math/Math.h
        Public/Math/MathTypes/Vector2.h
)

# ----------- Log Module ----------- #
set(LOOMCORE_LOG_SOURCES
        Public/Log/LogMessage.h
        Public/Log/LogLevels.h
        Public/Log/Log.h
        Public/Log/LogStack.h
        Public/Log/Sinks/ILogSink.h
        Public/Log/Sinks/ConsoleSink.h
        Public/Log/Sinks/FileSink.h

        Private/Log/Log.cpp
        Private/Log/LogStack.cpp
        Private/Log/Sinks/ILogSink.cpp
        Private/Log/Sinks/ConsoleSink.cpp
        Private/Log/Sinks/FileSink.cpp
)

# ----------- Event Module ----------- #
set(LOOMCORE_EVENT_SOURCES
        Public/Events/EventType.h
        Public/Events/EventDispatcher.h
        Public/Events/EventHandle.h

        Public/Events/Events/WindowEvents.h
        Public/Events/Events/KeyEvents.h
        Public/Events/Events/MouseEvents.h

        Private/Events/EventDispatcher.cpp
)

# ----------- Input Module ----------- #
set(LOOMCORE_INPUT_SOURCES

)

# ----------- Window Module ----------- #
set(LOOMCORE_WINDOW_SOURCES
        Public/Window/Window.h

        Private/Window/SDLWindow.cpp
        Private/Window/SDLWindow.h
)

# ----------- Utilities Module ----------- #
set(LOOMCORE_UTILITIES_SOURCES
        Private/Utilities/PlatformUtils.cpp
        Private/Utilities/PlatformUtils.h
)

# ----------- Platform Module ----------- #
set(LOOMCORE_PLATFORM_SOURCES)

if(WIN32)


elseif(APPLE)


elseif(UNIX)

endif()

# ----------- Minimum Required ----------- #
set(LOOMCORE_MINIMUM_SOURCES
        Public/LoomEngine.h
        Public/EntryPoint.h
)

# ----------- MASTER SOURCE LIST ----------- #

set(LOOM_ENGINE_SOURCES
        ${LOOMCORE_MINIMUM_SOURCES}
        ${LOOMCORE_CORE_SOURCES}
        ${LOOMCORE_MATH_SOURCES}
        ${LOOMCORE_LOG_SOURCES}
        ${LOOMCORE_EVENT_SOURCES}
        ${LOOMCORE_INPUT_SOURCES}
        ${LOOMCORE_WINDOW_SOURCES}
        ${LOOMCORE_UTILITIES_SOURCES}
        ${LOOMCORE_PLATFORM_SOURCES}
)

# ----------- Create Static Library ----------- #
add_library(LoomCore STATIC ${LOOM_ENGINE_SOURCES})
add_library(Loom::Core ALIAS LoomCore)
set_target_properties(LoomCore PROPERTIES EXPORT_NAME Core)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(LoomCore PRIVATE SDL3::SDL3)
target_compile_definitions(LoomCore PRIVATE SDL_MAIN_HANDLED)

# PUBLIC: anything linking LoomCore is compiled as C++20 as well.
target_compile_features(LoomCore PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(LoomCore PUBLIC /Zc:preprocessor)
endif()

# Public headers exposed to other projects
target_include_directories(LoomCore PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        $<INSTALL_INTERFACE:include/LoomCore>)

# Propagated to everything that links LoomCore
target_compile_definitions(LoomCore PUBLIC $<$<CONFIG:Debug>:LOOM_DEBUG>)
