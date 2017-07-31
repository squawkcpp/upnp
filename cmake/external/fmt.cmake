include(ExternalProject)

# ---------------------             FMT               ---------------------
ExternalProject_Add(
  cppfmt
  URL "https://github.com/fmtlib/fmt/archive/3.0.0.zip"
  CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS "cppfmt-prefix/src/cppfmt-build/fmt/libfmt.a"
)
ExternalProject_Get_Property(cppfmt binary_dir)
set(FMT_LIBRARY_PATH ${binary_dir}/fmt/${CMAKE_FIND_LIBRARY_PREFIXES}fmt.a )
ExternalProject_Get_Property(cppfmt source_dir)
set(FMT_INCLUDE_DIR "${source_dir}" )
set(FMT_LIBRARY fmt)
add_library(${FMT_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${FMT_LIBRARY} PROPERTY IMPORTED_LOCATION ${FMT_LIBRARY_PATH} )
add_dependencies(${FMT_LIBRARY} cppfmt)
