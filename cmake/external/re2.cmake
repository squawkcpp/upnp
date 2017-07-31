include(ExternalProject)
# - download and build RE2
# Once done this will define
#  RE2_INCLUDE_DIR - The RE2 include directory
#  RE2_LIBRARIES - The libraries needed to use RE2

ExternalProject_Add(
  google_re2
  URL "https://github.com/google/re2/archive/master.zip"
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS google_re2-prefix/src/google_re2-build/libre2.a
)
ExternalProject_Get_Property(google_re2 source_dir)
set(RE2_INCLUDE_DIR ${source_dir}/)
ExternalProject_Get_Property(google_re2 binary_dir)
set(RE2_LIBRARIES ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}re2.a)
set(RE2_LIBRARY re2)
add_library(${RE2_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${RE2_LIBRARY} PROPERTY IMPORTED_LOCATION ${RE2_LIBRARIES} )
add_dependencies(${RE2_LIBRARY} google_re2)
