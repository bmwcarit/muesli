message(STATUS "variable RAPIDJSON_VERSION_STRING=${RAPIDJSON_VERSION}")
message(STATUS "variable RAPIDJSON_INCLUDE_DIRS=${RAPIDJSON_INCLUDE_DIRS}")

if(NOT TARGET rapidjson::rapidjson)
    message(STATUS "rapidjson::rapidjson target not defined. Creating IMPORTED target.")
    add_library(rapidjson::rapidjson INTERFACE IMPORTED GLOBAL)
    set_property(TARGET rapidjson::rapidjson  PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${RAPIDJSON_INCLUDE_DIRS}
    )
endif(NOT TARGET rapidjson::rapidjson)

