option(
    USE_PLATFORM_GTEST_GMOCK
    "Resolve dependency to gtest and gmock from the system?"
    ON
)
message(STATUS "option USE_PLATFORM_GTEST_GMOCK=" ${USE_PLATFORM_GTEST_GMOCK})

include(AddGtestGmock)

AddTest(
    muesli-unit-test
    TestMain.cpp
    BaseArchiveTest.cpp
    TypeRegistryTest.cpp
    testtypes/TStruct.h
    testtypes/TStructExtended.h
    testtypes/TEnum.h
    testtypes/NestedStructs.h
    MockArchive.h
    TestUtil.h
    TestUtilTest.cpp
    TraitsTest.cpp
    IncrementalTypeListTest.cpp
    MockStream.h
    RegistryTest.cpp
    archives/json/JsonArchiveTest.cpp
    archives/json/JsonTest.cpp
    archives/json/TraitsTest.cpp
    archives/json/NullableTest.cpp
    archives/json/TupleTest.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(muesli-unit-test
    muesli
)

AddClangFormat(muesli-unit-test)
AddClangTidy(muesli-unit-test)
AddIncludeWhatYouUse(muesli-unit-test)
