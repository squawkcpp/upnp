ExternalProject_Add(
  lightning_master
  URL "https://github.com/lightningcpp/lightningcpp/archive/master.zip"
  DOWNLOAD_NO_PROGRESS 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(lightning_master source_dir)
set(LIGHTNING_INCLUDE_DIR ${source_dir}/include/)
set(LIGHTNING_LIBRARY lightning)
add_library(${LIGHTNING_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${LIGHTNING_LIBRARY} lightning_master )
