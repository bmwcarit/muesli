option(
    BUILD_MUESLI_UNIT_TESTS
    "Build muesli unit tests?"
    ON
)

if (BUILD_MUESLI_UNIT_TESTS)
    add_subdirectory(unit-tests)
endif(BUILD_MUESLI_UNIT_TESTS)


option(
    BUILD_MUESLI_PERFORMANCE_TESTS
    "Build muesli performance tests?"
    ON
)

if (BUILD_MUESLI_PERFORMANCE_TESTS)
    add_subdirectory(performance-tests)
endif(BUILD_MUESLI_PERFORMANCE_TESTS)
