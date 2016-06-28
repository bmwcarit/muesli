cmake_minimum_required(VERSION 2.8.11)

######## Project settings ########
project(muesli-0.1.0)
set(MUESLI_MAJOR_VERSION 0)
set(MUESLI_MINOR_VERSION 18)
set(MUESLI_PATCH_VERSION 0)
set(
    MUESLI_VERSION
    ${MUESLI_MAJOR_VERSION}.${MUESLI_MINOR_VERSION}.${MUESLI_PATCH_VERSION}
)

# add a fake target which allows displaying the headers in the IDE
FILE(GLOB_RECURSE MUESLI_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
add_custom_target(muesli_target_to_display_headers SOURCES ${MUESLI_HEADER_FILES})
AddClangFormat(muesli_target_to_display_headers)

add_library(muesli INTERFACE)

option(
    USE_PLATFORM_RAPIDJSON
    "Resolve dependency to rapidJSON from the system?"
    OFF
)
message(STATUS "option USE_PLATFORM_RAPIDJSON=" ${USE_PLATFORM_RAPIDJSON})

include(AddExternalProject)
if (USE_PLATFORM_RAPIDJSON)
    find_package(rapidjson REQUIRED)
    get_target_property(rapidjson_INCLUDE_DIRS rapidjson::rapidjson INTERFACE_INCLUDE_DIRECTORIES)
else(USE_PLATFORM_RAPIDJSON)
    message(STATUS "############ Download: rapidjson ############")

    AddExternalProject(
        rapidjson
        GIT_REPOSITORY https://github.com/miloyip/rapidjson
        GIT_TAG 369de87e5d7da05786731a712f25ab9b46c4b0ce
        TIMEOUT 10
        # Disable configure step
        CONFIGURE_COMMAND ""
        # Disable build step
        BUILD_COMMAND ""
    )

     ExternalProject_Get_Property(rapidjson SOURCE_DIR)
     set(rapidjson_INCLUDE_DIRS "${SOURCE_DIR}/include")
endif(USE_PLATFORM_RAPIDJSON)
include(CheckRapidJsonImportTargets)

target_include_directories(
    muesli
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<BUILD_INTERFACE:${rapidjson_INCLUDE_DIRS}>"
    "$<INSTALL_INTERFACE:include>"
)

target_compile_definitions(muesli INTERFACE RAPIDJSON_HAS_STDSTRING)

add_dependencies(muesli rapidjson::rapidjson)


install(
    TARGETS muesli
    EXPORT JoynrTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

option(
    BUILD_MUESLI_TESTS
    "Build muesli tests?"
    ON
)

if (BUILD_MUESLI_TESTS)
    add_subdirectory(tests)
endif(BUILD_MUESLI_TESTS)
