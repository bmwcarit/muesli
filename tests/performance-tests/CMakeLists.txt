option(
    USE_PLATFORM_GOOGLE_BENCHMARK
    "Resolve dependency to google-benchmark from the system?"
    OFF
)

find_package(Threads REQUIRED)

function(AddBenchmark TARGET)
    add_executable(
        ${TARGET}
        ${ARGN}
    )
    set_target_properties(
        ${TARGET}
        PROPERTIES
        COMPILE_FLAGS "-Wno-effc++ -Wno-unused-parameter"
    )
    target_link_libraries(
        ${TARGET}
        ${GBENCHMARK_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )
    target_include_directories(
        ${TARGET}
        SYSTEM
        PRIVATE
        ${GBENCHMARK_INCLUDE_DIRS}
    )
    if(NOT USE_PLATFORM_GOOGLE_BENCHMARK)
        add_dependencies(${TARGET} googlebenchmark)
    endif(NOT USE_PLATFORM_GOOGLE_BENCHMARK)
endfunction(AddBenchmark)


if (USE_PLATFORM_GOOGLE_BENCHMARK)
    # there is no FindGoogleBenchmark.cmake yet, TODO!
    # find_package(googlebenchmark REQUIRED)
else(USE_PLATFORM_GOOGLE_BENCHMARK)
    message(STATUS "############ Download: rapidjson ############")

    get_property(EP_PREFIX DIRECTORY PROPERTY EP_PREFIX)
    set(GBENCHMARK_LIBRARY_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX})

    AddExternalProject(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG master
        TIMEOUT 10
        CMAKE_ARGS "-DBENCHMARK_ENABLE_TESTING=OFF;-DCMAKE_BUILD_TYPE=Release"
        BUILD_BYPRODUCTS "${EP_PREFIX}/src/googlebenchmark-build/src/${GBENCHMARK_LIBRARY_NAME}"
        NO_PROPAGATE_CMAKE_ARGS
    )

     ExternalProject_Get_Property(googlebenchmark BINARY_DIR)
     set(GBENCHMARK_BINARY_DIR ${BINARY_DIR})

     ExternalProject_Get_Property(googlebenchmark SOURCE_DIR)
     set(GBENCHMARK_SOURCE_DIR ${SOURCE_DIR})

     set(GBENCHMARK_INCLUDE_DIRS ${GBENCHMARK_SOURCE_DIR}/include)
     set(GBENCHMARK_LIBRARIES ${GBENCHMARK_BINARY_DIR}/src/${GBENCHMARK_LIBRARY_NAME})
endif(USE_PLATFORM_GOOGLE_BENCHMARK)


AddBenchmark(
    muesli-benchmark
    JsonArchiveBenchmark.cpp
)

target_link_libraries(muesli-benchmark muesli)

AddClangFormat(muesli-benchmark)
