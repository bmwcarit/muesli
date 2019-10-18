cmake_minimum_required(VERSION 2.8.12)

######## Project settings ########
project(muesli-1.0.1)
set(muesli_MAJOR_VERSION 1)
set(muesli_MINOR_VERSION 0)
set(muesli_PATCH_VERSION 1)
set(
    muesli_VERSION
    ${muesli_MAJOR_VERSION}.${muesli_MINOR_VERSION}.${muesli_PATCH_VERSION}
)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "setting default value 'Debug' for CMAKE_BUILD_TYPE")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

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
include(AddClangTidy)
include(AddIncludeWhatYouUse)

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
    set(GNU_CXX_FLAGS " \
        -pipe -g \
        -Wunused-value \
        -Wunused \
        -Wpointer-arith \
        -Wformat-y2k \
        -Wno-format-extra-args \
        -Wformat-nonliteral \
        -Winit-self \
        -Winvalid-pch \
        -Wunsafe-loop-optimizations \
        -Wstack-protector \
        -Wswitch-enum \
        -Wfatal-errors \
        -D_GLIBCXX_ASSERTIONS \
        -fstack-protector-strong \
        -Wall \
        -Wextra \
        -fwrapv \
        -Wformat=2 \
        -Wformat-signedness \
        -Wmissing-include-dirs \
        -Wmissing-braces \
        -Wpacked \
        -Wimplicit-fallthrough=5 \
        -Wunused-parameter \
        -Wuninitialized \
        -Weffc++ \
        -Walloca \
        -Wduplicated-branches \
        -Wduplicated-cond \
        -Wfloat-equal \
        -Wshadow \
        -Wcast-qual \
        -Wsign-conversion \
        -Wlogical-op \
        -Wformat-security \
        -Walloc-zero \
        -Wcast-align \
        -Wdisabled-optimization \
        -Wredundant-decls \
        -Werror=all \
        -Werror=extra \
        -Werror=format \
        -Werror=format-signedness \
        -Werror=missing-include-dirs \
        -Werror=implicit-fallthrough \
        -Werror=unused-parameter \
        -Werror=uninitialized \
        -Werror=alloca \
        -Werror=duplicated-branches \
        -Werror=duplicated-cond \
        -Werror=float-equal \
        -Werror=cast-qual \
        -Werror=sign-conversion \
        -Werror=logical-op \
        -Werror=format-security \
        -Werror=alloc-zero \
        -Werror=cast-align \
        -Werror=redundant-decls \
    ")
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

if("${CMAKE_VERSION}" VERSION_LESS 3.0.0)
    message(STATUS "No support for INTERFACE library. An empty static library will be generated instead.")
    add_library(muesli STATIC ${MUESLI_HEADER_FILES})
    set_target_properties(muesli PROPERTIES LINKER_LANGUAGE CXX)
else("${CMAKE_VERSION}" VERSION_LESS 3.0.0)
    add_library(muesli INTERFACE)
endif("${CMAKE_VERSION}" VERSION_LESS 3.0.0)

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
    find_package(RapidJSON 1.1.0 REQUIRED)
else(USE_PLATFORM_RAPIDJSON)
    message(STATUS "############ Download: rapidjson ############")

    AddExternalProject(
        rapidjson
        GIT_REPOSITORY https://github.com/miloyip/rapidjson
        GIT_TAG v1.1.0
        TIMEOUT 10
        # Disable configure step
        CONFIGURE_COMMAND ""
        # Disable build step
        BUILD_COMMAND ""
    )

    # add import target for rapidjson library
    include(CreateImportTarget)
    CreateImportTarget(rapidjson::rapidjson)

    # make sure rapidjson external project is built in advance
    add_dependencies(rapidjson::rapidjson rapidjson)

    # set include directories properly
    ExternalProject_Get_Property(rapidjson SOURCE_DIR)
    set_property(
        TARGET rapidjson::rapidjson
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}/include"
    )
endif(USE_PLATFORM_RAPIDJSON)
include(CheckRapidJsonImportTargets)

# verify custom rapidjson target does include correct INCLUDES
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
        "CMake/CreateImportTarget.cmake"
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
