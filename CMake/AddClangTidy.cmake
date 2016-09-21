option(
    ENABLE_CLANG_TIDY
    "Use clang-tidy for code analysis/cleanup?"
    OFF
)

message(STATUS "option ENABLE_CLANG_TIDY=" ${ENABLE_CLANG_TIDY})

if(${ENABLE_CLANG_TIDY})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    LocateProgram(clang-tidy CLANG_TIDY_PATH)
    add_custom_target(tidy)

    set(
        CLANG_TIDY_OPTIONS
        -checks=*
        -header-filter=.*muesli.*
        CACHE LIST "additional options for clang-tidy")

    set(CLANG_TIDY_TARGET_FILES "" CACHE LIST "files which will be cleaned up by clang-tidy")
    option(CLANG_TIDY_APPLY_FIXES "files which will be cleaned up by clang-tidy" ON)
    option(CLANG_TIDY_FORMAT "format files after applying fixes" ON)

    set(CLANG_TIDY_FIXES_PATH "${CMAKE_BINARY_DIR}/tidy-fixes/"
        CACHE FILEPATH "location of exported fixes from 'clang-tidy'")
    file(MAKE_DIRECTORY ${CLANG_TIDY_FIXES_PATH})

    if(${CLANG_TIDY_APPLY_FIXES})
        LocateProgram(clang-apply-replacements CLANG_APPLY_REPLACEMENTS_PATH)

        set(CLANG_APPLY_REPLACEMENTS_OPTIONS "-remove-change-desc-files")
        if(${CLANG_TIDY_FORMAT})
            set(CLANG_APPLY_REPLACEMENTS_OPTIONS ${CLANG_APPLY_REPLACEMENTS_OPTIONS} "-format")
        endif(${CLANG_TIDY_FORMAT})

        add_custom_command(TARGET tidy
                           POST_BUILD
                           COMMAND ${CLANG_APPLY_REPLACEMENTS_PATH}
                                   ${CLANG_APPLY_REPLACEMENTS_OPTIONS}
                                   ${CLANG_TIDY_FIXES_PATH}
                           VERBATIM
        )
    endif(${CLANG_TIDY_APPLY_FIXES})

endif(${ENABLE_CLANG_TIDY})

function(FilterOutHeaders INPUT OUTPUT)
     set(FILTERED_LIST "")
     foreach(ENTRY ${${INPUT}})
       if(NOT ${ENTRY} MATCHES ".+\\.h$")
           list(APPEND FILTERED_LIST ${ENTRY})
       endif()
     endforeach()
     set(${OUTPUT} ${FILTERED_LIST} PARENT_SCOPE)
endfunction(FilterOutHeaders INPUT OUTPUT)


function(AddClangTidy TARGET)
    if(${ENABLE_CLANG_TIDY})

        get_property(TARGET_SOURCES TARGET ${TARGET} PROPERTY SOURCES)
        FilterOutHeaders(TARGET_SOURCES TARGET_SOURCES_WO_HEADERS)
        list(APPEND ${CLANG_TIDY_TARGET_FILES} ${TARGET_SOURCES_WO_HEADERS})

        set(TIDY_TARGET "tidy-${TARGET}")
        add_custom_target(${TIDY_TARGET})
        add_dependencies(tidy ${TIDY_TARGET})

        foreach(TARGET_FILE ${TARGET_SOURCES_WO_HEADERS})
            set(TARGET_FILE_EXPORT_FILE "${CLANG_TIDY_FIXES_PATH}/${TARGET_FILE}.yaml")
            # make sure the folder exists
            get_filename_component(TARGET_FILE_EXPORT_FOLDER ${TARGET_FILE_EXPORT_FILE} DIRECTORY)
            file(MAKE_DIRECTORY ${TARGET_FILE_EXPORT_FOLDER})
            # call 'clang-tidy' for each source file of this target
            add_custom_command(
                 TARGET ${TIDY_TARGET}
                 COMMAND ${CLANG_TIDY_PATH} -p ${CMAKE_BINARY_DIR}
                                            -export-fixes ${TARGET_FILE_EXPORT_FILE}
                                            ${CLANG_TIDY_OPTIONS}
                                            ${TARGET_FILE}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                 VERBATIM
            )

        endforeach(TARGET_FILE ${TARGET_SOURCES_WO_HEADERS})
    endif(${ENABLE_CLANG_TIDY})

endfunction(AddClangTidy)
