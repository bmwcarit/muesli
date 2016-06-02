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

add_library(muesli INTERFACE)

target_include_directories(
    muesli
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)
install(
    TARGETS muesli
    EXPORT JoynrTargets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

add_subdirectory(tests)
