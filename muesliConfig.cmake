include("${CMAKE_CURRENT_LIST_DIR}/muesliTargets.cmake")

find_package(RapidJSON 1.0.2 REQUIRED)

if(NOT TARGET rapidjson::rapidjson)
    include("${CMAKE_CURRENT_LIST_DIR}/CreateImportTarget.cmake")
    CreateImportTarget(rapidjson::rapidjson)
    set_property(
        TARGET rapidjson::rapidjson
        APPEND
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RAPIDJSON_INCLUDE_DIRS}
    )
endif()

set_property(
    TARGET muesli::muesli
    APPEND
    PROPERTY INTERFACE_LINK_LIBRARIES rapidjson::rapidjson
)

set_property(
    TARGET muesli::muesli
    APPEND
    PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${RAPIDJSON_INCLUDE_DIRS}
)
