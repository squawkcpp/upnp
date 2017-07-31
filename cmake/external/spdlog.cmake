include(ExternalProject)

ExternalProject_Add(
  spdlog_master
  URL "https://github.com/gabime/spdlog/archive/master.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(spdlog_master source_dir)
set(SPDLOG_INCLUDE_DIR ${source_dir}/include/)
set(SPDLOG_LIBRARY spdlog)
add_library(${SPDLOG_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${SPDLOG_LIBRARY} spdlog_master )
