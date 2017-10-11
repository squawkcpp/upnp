# -------------------------------------------------------------------------
# Build UPNP server
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

project( upnp )

SET(UPNP_TAG_VERSION "master" CACHE STRING "The tagged version.")

# -------------------------------------------------------------------------
# -----------------------   Options and Parameters  -----------------------
# -------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
option(build_tests "Build all squawk unit tests." ON)

# -------------------------------------------------------------------------
# -----------------------   Config Toolchain        -----------------------
# -------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++14)
endif()

add_definitions(-DASIO_STANDALONE)
add_definitions(-DUPNP_VERSION="${UPNP_TAG_VERSION}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSPDLOG_TRACE_ON)
    add_definitions(-DSPDLOG_DEBUG_ON)
    add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

#add headers for editor
FILE(GLOB_RECURSE HeaderFiles "${PROJECT_SOURCE_DIR}/src/*.h")
add_custom_target(upnp_headers SOURCES ${HeaderFiles})

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------
include(ExternalProject)
include(cmake/external/re2.cmake)
include(cmake/external/gtest.cmake)
include(cmake/external/asio.cmake)
include(cmake/external/fmt.cmake)
include(cmake/external/lightning.cmake)
include(cmake/external/rapidxml_ns.cmake)
include(cmake/external/rapidjson.cmake)
include(cmake/external/cxxopts.cmake)
include(cmake/external/redox.cmake)
include(cmake/external/spdlog.cmake)

SET( INCLUDES ${INCLUDES} ${GTEST_INCLUDE_DIR} ${FMT_INCLUDE_DIR}
            ${RE2_INCLUDE_DIR} ${ASIO_INCLUDE_DIR}
            ${LIGHTNING_INCLUDE_DIR} ${RAPIDXML_INCLUDE_DIR}
            ${CXXOPTS_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR}
            ${RAPIDJSON_INCLUDE_DIR} ${REDOX_INCLUDE_DIR})

# -------------------------------------------------------------------------
# -----------------------   Find Dependencies       -----------------------
# -------------------------------------------------------------------------
find_package(Boost 1.54.0 COMPONENTS filesystem system)
#find_package(CURLpp REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Hiredis)
find_package(Libev)

SET( INCLUDES ${INCLUDES}
    ${Boost_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${OPENSSL_INCLUDES}
    ${FLAC++_INCLUDE_DIR}
)

SET( LIBS ${LIBS}
    ${Boost_LIBRARIES} ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES} ${LIBHIREDIS_LIBRARIES} ${LIBEV_LIBRARIES}
    -lpthread -luuid -lm -lz
)

# -------------------------------------------------------------------------
# -----------------------   Compile Binary          -----------------------
# -------------------------------------------------------------------------
aux_source_directory( ${PROJECT_SOURCE_DIR}/src UPNP_SOURCES )
include_directories(${ROOT} ${INCLUDES} ${GTEST_INCLUDE_DIR})
add_executable(upnp ${PROJECT_SOURCE_DIR}/main.cpp ${UPNP_SOURCES})
target_link_libraries(upnp cxxopts re2 lightning redox fmt spdlog asio rapidjson rapidxml ${LIBS})
install(TARGETS upnp RUNTIME DESTINATION bin)

# -------------------------------------------------------------------------
# -----------------------   Manage Test Cases       -----------------------
# -------------------------------------------------------------------------
if (build_tests)
    aux_source_directory( ${PROJECT_SOURCE_DIR}/test/ UPNP_SOURCES_TEST )
    enable_testing()
    include_directories(${ROOT} ${INCLUDES} ${GTEST_INCLUDE_DIR})
    add_executable(test_upnp ${UPNP_SOURCES_TEST} ${UPNP_SOURCES})
    target_link_libraries(test_upnp ${GTEST_MAIN_PATH} gtest re2 lightning redox fmt spdlog asio rapidjson rapidxml ${LIBS})
    add_test(upnp-tests test_upnp)
endif()

# -------------------------------------------------------------------------
# -----------------------   Generate Package        -----------------------
# -------------------------------------------------------------------------
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SET_DESTDIR "on")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
SET(CPACK_GENERATOR "DEB")

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

SET(CPACK_PACKAGE_DESCRIPTION "loudlabs media server")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "squawk upnp server.")
SET(CPACK_PACKAGE_VENDOR "loudlabs")
SET(CPACK_PACKAGE_CONTACT "squawkcpp@gmail.com")

if(UPNP_TAG_VERSION MATCHES "^([0-9]+)[.]([0-9]+)[.]([0-9]+)$" )
    string(REPLACE "." ";" UPNP_TAG_VERSION_STRING ${UPNP_TAG_VERSION})
    list(GET UPNP_TAG_VERSION_STRING 0 MAJOR_VERSION)
    list(GET UPNP_TAG_VERSION_STRING 1 MINOR_VERSION)
    list(GET UPNP_TAG_VERSION_STRING 2 PATCH_VERSION)
else()
    SET(MAJOR_VERSION "0")
    SET(MINOR_VERSION "0")
    SET(PATCH_VERSION "0")
endif()
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")

SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${UPNP_TAG_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${UPNP_TAG_VERSION}")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libpcrecpp0v5")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "media")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
SET(CPACK_COMPONENTS_ALL applications Libraries ApplicationData)
INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
