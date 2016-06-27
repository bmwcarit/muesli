option(
    BUILD_MUESLI_UNIT_TESTS
    "Build muesli unit tests?"
    ON
)

if (BUILD_MUESLI_UNIT_TESTS)
    add_subdirectory(unit-tests)
endif(BUILD_MUESLI_UNIT_TESTS)
