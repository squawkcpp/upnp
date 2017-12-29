ExternalProject_Add(
  asio_master
  URL "https://github.com/chriskohlhoff/asio/archive/master.zip"
  DOWNLOAD_NO_PROGRESS 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(asio_master source_dir)
set(ASIO_INCLUDE_DIR ${source_dir}/asio/include/)
set(ASIO_LIBRARY asio)
add_library(${ASIO_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${ASIO_LIBRARY} asio_master )
