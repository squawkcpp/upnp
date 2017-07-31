# -------------------------------------------------------------------------
# Build UPNP server
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.0)

project( upnp )

#add headers for editor
FILE(GLOB_RECURSE HeaderFiles "${PROJECT_SOURCE_DIR}/*.h")
add_custom_target(cds_headers SOURCES ${HeaderFiles})

# -------------------------------------------------------------------------
# -----------------------   Compile Binary          -----------------------
# -------------------------------------------------------------------------
aux_source_directory( ${PROJECT_SOURCE_DIR}/src UPNP_SOURCES )
include_directories(${ROOT} ${INCLUDES} ${GTEST_INCLUDE_DIR})
add_library(upnp ${UPNP_SOURCES})

# -------------------------------------------------------------------------
# -----------------------   Manage Test Cases       -----------------------
# -------------------------------------------------------------------------
if (build_tests)
    aux_source_directory( test/ UPNP_SOURCES_TEST )
    enable_testing()
    include_directories(${ROOT} ${INCLUDES} ${GTEST_INCLUDE_DIR})
    add_executable(test_cds ${UPNP_SOURCES_TEST} ${UPNP_SOURCES} ${UPNP_SOURCES_MODULES} ${UPNP_SOURCES_UTILS})
    target_link_libraries(test_cds ${GTEST_MAIN_PATH} cxxopts re2 lightning rapidjson rapidxml
        LibAvCpp LibImageCpp redox fmt exif spdlog gtest ${LIBS} )
    add_test(cds-tests test_cds)
endif()
