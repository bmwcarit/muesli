include(AddGtestGmock)

include(AddExternalProject)
if (USE_PLATFORM_RAPIDJSON)
    find_package(rapidjson REQUIRED)
    get_target_property(rapidjson_INCLUDE_DIRS rapidjson::rapidjson INTERFACE_INCLUDE_DIRECTORIES)
else(USE_PLATFORM_RAPIDJSON)
    message(STATUS "############ Download: rapidjson ############")

    AddExternalProject(
        rapidjson
        GIT_REPOSITORY https://github.com/miloyip/rapidjson
        GIT_TAG master
        TIMEOUT 10
        # Disable configure step
        CONFIGURE_COMMAND ""
        # Disable build step
        BUILD_COMMAND ""
    )

     ExternalProject_Get_Property(rapidjson SOURCE_DIR)
     set(rapidjson_INCLUDE_DIRS "${SOURCE_DIR}/include")
endif(USE_PLATFORM_RAPIDJSON)

message(STATUS "variable rapidjson_INCLUDE_DIRS=" ${rapidjson_INCLUDE_DIRS})

AddTest(
    muesli-unit-test
    TestMain.cpp
    StringIStreamTest.cpp
    StringOStreamTest.cpp
    BaseArchiveTest.cpp
    JsonArchiveTest.cpp
    JsonTest.cpp
    OutputStreamTest.cpp
    InputStreamTest.cpp
    StdOStreamWrapperTest.cpp
    StdIStreamWrapperTest.cpp
    archives/json/TraitsTest.cpp
    TypeRegistryTest.cpp
    testtypes/TStruct.h
    testtypes/TStructExtended.h
    testtypes/TEnum.h
    MockArchive.h
    TestUtil.h
    TestUtilTest.cpp
    TraitsTest.cpp
    IncrementalTypeListTest.cpp
    MockStream.h
    TupleTest.cpp
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set_target_properties(muesli-unit-test PROPERTIES COMPILE_FLAGS "-Wall -g -Wextra -pedantic -Wno-effc++")
endif()

target_link_libraries(
    muesli-unit-test
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
)

if(NOT USE_PLATFORM_RAPIDJSON)
    add_dependencies(muesli-unit-test rapidjson)
endif(NOT USE_PLATFORM_RAPIDJSON)

target_include_directories(
    muesli-unit-test
    SYSTEM
    PRIVATE
    ${rapidjson_INCLUDE_DIRS}
)

target_link_libraries(muesli-unit-test
    muesli
)

AddClangFormat(muesli-unit-test)
