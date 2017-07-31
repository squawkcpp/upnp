include(ExternalProject)

# ---------------------          cxxopts            ---------------------
ExternalProject_Add(
  cxxopts_master
  URL "https://github.com/jarro2783/cxxopts/archive/master.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(cxxopts_master source_dir)
set(CXXOPTS_INCLUDE_DIR ${source_dir}/include/)
set(CXXOPTS_LIBRARY cxxopts)
add_library(${CXXOPTS_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${CXXOPTS_LIBRARY} cxxopts_master )
