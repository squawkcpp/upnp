# ---------------------             hiredis               ---------------------
ExternalProject_Add(
  hiredis
  DEPENDS libev_main
  URL "https://github.com/redis/hiredis/archive/master.zip"
  DOWNLOAD_NO_PROGRESS 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND cd <SOURCE_DIR> && make && make install
  INSTALL_COMMAND "" #cd <SOURCE_DIR> && make install
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS "hiredis-prefix/src/hiredis/libhiredis.a"
)
ExternalProject_Get_Property(hiredis binary_dir)
ExternalProject_Get_Property(hiredis source_dir)
set(HIREDIS_LIBRARY ${source_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}hiredis.a )
set(HIREDIS_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/hiredis-prefix/src/ )
set(HIREDIS_LIBRARY_NAME hredis)
add_library(${HIREDIS_LIBRARY_NAME} UNKNOWN IMPORTED)
set_property(TARGET ${HIREDIS_LIBRARY_NAME} PROPERTY IMPORTED_LOCATION ${HIREDIS_LIBRARY} )
add_dependencies(${HIREDIS_LIBRARY_NAME} hiredis)
