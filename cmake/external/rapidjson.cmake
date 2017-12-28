#https://github.com/miloyip/rapidjson
ExternalProject_Add(
  rapidjson_master
  URL "https://github.com/miloyip/rapidjson/archive/master.zip"
  DOWNLOAD_NO_PROGRESS 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(rapidjson_master source_dir)
set(RAPIDJSON_INCLUDE_DIR ${source_dir}/include/)
set(RAPIDJSON_LIBRARY rapidjson)
add_library(${RAPIDJSON_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${RAPIDJSON_LIBRARY} rapidjson_master )

