#https://github.com/svgpp/rapidxml_ns
ExternalProject_Add(
  rapidxml_master
  URL "https://github.com/svgpp/rapidxml_ns/archive/master.zip"
  DOWNLOAD_NO_PROGRESS 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(rapidxml_master source_dir)
set(RAPIDXML_INCLUDE_DIR ${source_dir})
set(RAPIDXML_LIBRARY rapidxml)
add_library(${RAPIDXML_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${RAPIDXML_LIBRARY} rapidxml_master )
