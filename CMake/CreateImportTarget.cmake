function(CreateImportTarget TARGET)
    if("${CMAKE_VERSION}" VERSION_LESS 3.0.0)
        message(
            STATUS
            "No support for INTERFACE library. An empty static library will be generated instead."
        )
        add_library("${TARGET}" STATIC IMPORTED GLOBAL)
    else("${CMAKE_VERSION}" VERSION_LESS 3.0.0)
        add_library("${TARGET}" INTERFACE IMPORTED GLOBAL)
    endif("${CMAKE_VERSION}" VERSION_LESS 3.0.0)
endfunction(CreateImportTarget)
