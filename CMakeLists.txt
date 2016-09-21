cmake_minimum_required(VERSION 2.8.12)

######## Project settings ########
project(muesli-0.1.3)
set(muesli_MAJOR_VERSION 0)
set(muesli_MINOR_VERSION 1)
set(muesli_PATCH_VERSION 3)
set(
    muesli_VERSION
    ${muesli_MAJOR_VERSION}.${muesli_MINOR_VERSION}.${muesli_PATCH_VERSION}
)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "setting default value 'Debug' for CMAKE_BUILD_TYPE")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

set_property(
    CACHE
    CMAKE_BUILD_TYPE
    PROPERTY STRINGS
    "Debug"
    "Release"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(Sanitizers)


# defines GNU standard installation directories
include(GNUInstallDirs)

include(LocateProgram)
include(AddClangFormat)
message(STATUS "variable CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

######## Add Boost ########

find_package(Boost 1.58.0 REQUIRED
    COMPONENTS
    system
)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

######## DEFAULT COMPILER FLAGS #############

include(SetCppStandard)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GNU_CXX_FLAGS "-pipe -g")
    set(GNU_CXX_FLAGS "${GNU_CXX_FLAGS} -Wall -Wunused-value -Wunused -Wcast-qual -Wpointer-arith")
    set(GNU_CXX_FLAGS "${GNU_CXX_FLAGS} -Wextra -Wcast-align -Wdisabled-optimization -Wformat-y2k")
    set(GNU_CXX_FLAGS "${GNU_CXX_FLAGS} -Wno-format-extra-args -Wformat-nonliteral -Wformat=2 -Winit-self")
    set(GNU_CXX_FLAGS "${GNU_CXX_FLAGS} -Winvalid-pch -Wunsafe-loop-optimizations -Wmissing-include-dirs")
    set(GNU_CXX_FLAGS "${GNU_CXX_FLAGS} -Wmissing-braces -Wpacked -Wredundant-decls -Wstack-protector")
    set(GNU_CXX_FLAGS "${GNU_CXX_FLAGS} -Wswitch-enum -Wuninitialized -Weffc++ -Wformat-security")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNU_CXX_FLAGS} -pthread")

    # check GNU CXX compiler version
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.9" OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.9")
        add_definitions(-D_GLIBCXX_USE_NANOSLEEP)
    else()
        message(FATAL_ERROR "a gcc compiler with a version higher than 4.9 is needed.")
    endif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.9" OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.9")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

set(MUESLI_BINARY_DIR ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MUESLI_BINARY_DIR})

# add a fake target which allows displaying the headers in the IDE
FILE(GLOB_RECURSE MUESLI_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
add_custom_target(muesli_target_to_display_headers SOURCES ${MUESLI_HEADER_FILES})
AddClangFormat(muesli_target_to_display_headers)

if("${CMAKE_VERSION}" VERSION_GREATER 3.0.1)
    add_library(muesli INTERFACE)
else("${CMAKE_VERSION}" VERSION_GREATER 3.0.1)
    message(STATUS "No support for INTERFACE library. An empty static library will be generated instead.")
    add_library(muesli STATIC ${MUESLI_HEADER_FILES})
    set_target_properties(muesli PROPERTIES LINKER_LANGUAGE CXX)
endif("${CMAKE_VERSION}" VERSION_GREATER 3.0.1)

set_property(
    TARGET muesli
    PROPERTY INTERFACE_muesli_MAJOR_VERSION ${muesli_MAJOR_VERSION}
)
set_property(
    TARGET muesli
    PROPERTY INTERFACE_muesli_MINOR_VERSION ${muesli_MINOR_VERSION}
)
set_property(
    TARGET muesli
    PROPERTY INTERFACE_muesli_PATCH_VERSION ${muesli_PATCH_VERSION}
)
set_property(
    TARGET muesli
    PROPERTY INTERFACE_muesli_VERSION ${muesli_VERSION}
)
set_property(
    TARGET muesli
    APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING muesli_VERSION
)

option(
    USE_PLATFORM_RAPIDJSON
    "Resolve dependency to rapidJSON from the system?"
    OFF
)
message(STATUS "option USE_PLATFORM_RAPIDJSON=" ${USE_PLATFORM_RAPIDJSON})

include(AddExternalProject)
if (USE_PLATFORM_RAPIDJSON)
    find_package(RapidJSON 1.0.2 REQUIRED)
else(USE_PLATFORM_RAPIDJSON)
    message(STATUS "############ Download: rapidjson ############")

    AddExternalProject(
        rapidjson
        GIT_REPOSITORY https://github.com/miloyip/rapidjson
        GIT_TAG ab791ae90cbfd2e229be1b5e9db577728cfb3352
        TIMEOUT 10
        # Disable configure step
        CONFIGURE_COMMAND ""
        # Disable build step
        BUILD_COMMAND ""
    )

    ExternalProject_Get_Property(rapidjson SOURCE_DIR)
    # add import target for rapidjson library
    add_library(rapidjson::rapidjson INTERFACE IMPORTED GLOBAL)
    add_dependencies(rapidjson::rapidjson rapidjson)
    set_property(
        TARGET rapidjson::rapidjson
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}/include"
    )
endif(USE_PLATFORM_RAPIDJSON)
include(CheckRapidJsonImportTargets)
get_target_property(rapidjson_INCLUDE_DIRECTORIES rapidjson::rapidjson INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "rapidjson_INCLUDE_DIRECTORIES=${rapidjson_INCLUDE_DIRECTORIES}")

target_include_directories(
    muesli
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(muesli INTERFACE rapidjson::rapidjson)

add_dependencies(muesli rapidjson::rapidjson)

# defines GNU standard installation directories
include(GNUInstallDirs)

install(
    TARGETS muesli
    EXPORT muesliTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/muesli/muesliConfigVersion.cmake"
    VERSION ${muesli_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(
    EXPORT muesliTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/muesli/muesliTargets.cmake"
    NAMESPACE muesli::
)
configure_file(
    muesliConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/muesli/muesliConfig.cmake"
    COPYONLY
)

set(CmakePackageDir ${CMAKE_INSTALL_LIBDIR}/cmake/muesli)

install(
    EXPORT muesliTargets
    FILE muesliTargets.cmake
    NAMESPACE muesli::
    DESTINATION ${CmakePackageDir}
)
install(
    FILES
        muesliConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/muesli/muesliConfigVersion.cmake"
    DESTINATION ${CmakePackageDir}
    COMPONENT Devel
)

option(
    BUILD_MUESLI_TESTS
    "Build muesli tests?"
    ON
)

if (BUILD_MUESLI_TESTS)
    add_subdirectory(tests)
endif(BUILD_MUESLI_TESTS)
