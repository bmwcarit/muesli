# include-what-you-use can be used with CMake 3.3.0 and higher
if("${CMAKE_VERSION}" VERSION_GREATER 3.3.0)

    option(
        ENABLE_INCLUDE_WHAT_YOU_USE
        "Use include-what-you-use to identify missing/superfluous includes?"
        OFF
    )

    message(STATUS "option ENABLE_INCLUDE_WHAT_YOU_USE=" ${ENABLE_INCLUDE_WHAT_YOU_USE})

    if(${ENABLE_INCLUDE_WHAT_YOU_USE})
        LocateProgram(include-what-you-use INCLUDE_WHAT_YOU_USE_PATH)
        set(INCLUDE_WHAT_YOU_USE_OPTIONS ""
            CACHE STRING "additional options for include-what-you-use")

        set(INCLUDE_WHAT_YOU_USE_PATH_AND_OPTIONS
            ${INCLUDE_WHAT_YOU_USE_PATH}
            ${INCLUDE_WHAT_YOU_USE_OPTIONS}
        )
    endif(${ENABLE_INCLUDE_WHAT_YOU_USE})

endif("${CMAKE_VERSION}" VERSION_GREATER 3.3.0)

function(AddIncludeWhatYouUse TARGET)
    if(${ENABLE_INCLUDE_WHAT_YOU_USE})
        set_property(TARGET ${TARGET}
                     PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE_PATH_AND_OPTIONS})
    endif(${ENABLE_INCLUDE_WHAT_YOU_USE})
endfunction(AddIncludeWhatYouUse)

