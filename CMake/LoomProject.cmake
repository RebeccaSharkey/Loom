function(loom_read_project DESCRIPTOR OUT_MODULES)
    if(NOT EXISTS "${DESCRIPTOR}")
        message(FATAL_ERROR "Loom: project descriptor not found: ${DESCRIPTOR}")
    endif()

    # Make CMake re-run configure whenever the descriptor changes.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${DESCRIPTOR}")

    file(READ "${DESCRIPTOR}" _json)

    string(JSON _count ERROR_VARIABLE _err LENGTH "${_json}" Modules)
    if(_err)
        set(_count 0)
    endif()

    set(_modules "")
    if(_count GREATER 0)
        math(EXPR _last "${_count} - 1")
        foreach(_i RANGE ${_last})
            string(JSON _mod GET "${_json}" Modules ${_i})
            list(APPEND _modules "${_mod}")
        endforeach()
    endif()

    set(${OUT_MODULES} "${_modules}" PARENT_SCOPE)
endfunction()