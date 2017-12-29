# -------------------------------------------------------------------------
# Build UPNP server tests
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

project( upnp_test )

aux_source_directory( ${PROJECT_SOURCE_DIR} UPNP_SOURCES_TEST )
include_directories(${ROOT} ${IMPORTED_INCLUDES})
add_executable(${PROJECT_NAME}  ${UPNP_SOURCES_TEST})
target_link_libraries(${PROJECT_NAME} ${GTEST_MAIN_PATH} gtest upnp_app)
add_test(upnp_tests ${PROJECT_NAME})
