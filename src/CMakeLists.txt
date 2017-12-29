# -------------------------------------------------------------------------
# Build UPNP server app
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

project( upnp_app )

aux_source_directory( ${PROJECT_SOURCE_DIR} UPNP_SOURCES )
include_directories(${ROOT} ${IMPORTED_INCLUDES})
add_library(${PROJECT_NAME} ${UPNP_SOURCES})
target_link_libraries(${PROJECT_NAME}
        re2 lightning spdlog fmt asio
        rapidjson rapidxml redox
        ${CURL_LIBRARIES} ${LIBEV_LIBRARIES}
        hredis libev boost filesystem system ssl crypto Threads::Threads -ldl
)
