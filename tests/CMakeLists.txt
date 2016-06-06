include(AddGtestGmock)

AddTest(
    muesli-unit-test
    TestMain.cpp
    BaseArchiveTest.cpp
    OutputStreamTest.cpp
    InputStreamTest.cpp
    StdOStreamWrapperTest.cpp
)

target_link_libraries(
    muesli-unit-test
    ${GTEST_LIBRARIES}
    ${GMOCK_LIBRARIES}
)

target_include_directories(
    muesli-unit-test
    SYSTEM
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)
AddClangFormat(muesli-unit-test)
