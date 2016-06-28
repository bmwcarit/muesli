include(AddGtestGmock)

message(STATUS "variable rapidjson_INCLUDE_DIRS=" ${rapidjson_INCLUDE_DIRS})

AddTest(
    muesli-unit-test
    TestMain.cpp
    BaseArchiveTest.cpp
    JsonArchiveTest.cpp
    JsonTest.cpp
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
    RegistryTest.cpp
    streams/StringIStreamTest.cpp
    streams/StringOStreamTest.cpp
    streams/OutputStreamTest.cpp
    streams/InputStreamTest.cpp
    streams/StdOStreamWrapperTest.cpp
    streams/StdIStreamWrapperTest.cpp
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
