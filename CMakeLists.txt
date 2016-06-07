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

target_include_directories(
    muesli
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

target_compile_definitions(muesli INTERFACE RAPIDJSON_HAS_STDSTRING)

install(
    TARGETS muesli
    EXPORT JoynrTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

add_subdirectory(tests)
