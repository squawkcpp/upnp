# ---------------------             FMT               ---------------------
ExternalProject_Add(
  cppfmt
  URL "https://github.com/fmtlib/fmt/archive/4.1.0.zip"
  DOWNLOAD_NO_PROGRESS 1
  CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  BUILD_IN_SOURCE 1
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  LOG_BUILD 1
  LOG_TEST 1
#  BUILD_BYPRODUCTS "cppfmt-prefix/src/cppfmt-build/fmt/libfmt.a"
)
ExternalProject_Get_Property(cppfmt binary_dir)
#set(FMT_LIBRARY_PATH ${binary_dir}/fmt/${CMAKE_FIND_LIBRARY_PREFIXES}fmt.a )
ExternalProject_Get_Property(cppfmt source_dir)
set(FMT_INCLUDE_DIR "${source_dir}" )
set(FMT_LIBRARY fmt)
add_library(${FMT_LIBRARY} INTERFACE IMPORTED)
#set_property(TARGET ${FMT_LIBRARY} PROPERTY IMPORTED_LOCATION ${FMT_LIBRARY_PATH} )
add_dependencies(${FMT_LIBRARY} cppfmt)
