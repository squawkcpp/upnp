# -------------------------------------------------------------------------
# Build UPNP server
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

project( upnp )

SET(UPNP_TAG_VERSION "master" CACHE STRING "The tagged version.")

# -----------------------   Options and Parameters  -----------------------

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
option(build_tests "Build all squawk unit tests." ON)

set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_BUILD_MESSAGE LAZY)

# -----------------------   Config Toolchain        -----------------------

include(CTest)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

link_libraries("-static")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

add_definitions( -DHTTP_BUFFER_SIZE=4096 -DHTTP_CLIENT_TIMEOUT=5
                 -DHTTP_SERVER_THREAD_POOL_SIZE=10 -DASIO_STANDALONE
                 -DUPNP_VERSION="${UPNP_TAG_VERSION}" -DSPDLOG_FMT_EXTERNAL
                 -DFMT_HEADER_ONLY)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSPDLOG_TRACE_ON)
    add_definitions(-DSPDLOG_DEBUG_ON)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# ---------------------   External Dependencies       ---------------------

include(ExternalProject)
include(cmake/external/openssl.cmake)
include(cmake/external/boost.cmake)
include(cmake/external/libev.cmake)
include(cmake/external/hiredis.cmake)
include(cmake/external/redox.cmake)
include(cmake/external/re2.cmake)
include(cmake/external/gtest.cmake)
include(cmake/external/asio.cmake)
include(cmake/external/fmt.cmake)
include(cmake/external/lightning.cmake)
include(cmake/external/rapidxml_ns.cmake)
include(cmake/external/rapidjson.cmake)
include(cmake/external/cxxopts.cmake)
include(cmake/external/spdlog.cmake)

SET(IMPORTED_INCLUDES
    ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${LIBEV_INCLUDE_DIR}
    ${HIREDIS_INCLUDE_DIR} ${REDOX_INCLUDE_DIR} ${RE2_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR} ${ASIO_INCLUDE_DIR} ${FMT_INCLUDE_DIR}
    ${LIGHTNING_INCLUDE_DIR} ${RAPIDXML_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIR}
    ${CXXOPTS_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR})

# -----------------------           Compile           -----------------------

add_subdirectory(src)

if (build_tests)
    add_subdirectory(test)
endif()

include_directories(${ROOT} ${IMPORTED_INCLUDES})
add_executable(upnp ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(upnp upnp_app cxxopts)
install(TARGETS upnp RUNTIME DESTINATION bin)

# -----------------------   Generate Package        -----------------------

IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${UPNP_TAG_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${UPNP_TAG_VERSION}")
INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
