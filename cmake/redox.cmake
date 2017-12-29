ExternalProject_Add(
  redox_master
  DEPENDS hiredis
  URL "https://github.com/hmartiro/redox/archive/master.zip"
  DOWNLOAD_NO_PROGRESS 1
  CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -Dlib=off -DLIBEV_ROOT_DIR=${CMAKE_CURRENT_BINARY_DIR}/redox_master-prefix/src/redox_master/ -DLIBEV_LIBRARY=${LIBEV_LIBRARY} -DLIBEV_INCLUDE_DIR=${LIBEV_INCLUDE_DIR} -DHIREDIS_LIBRARY=${HIREDIS_LIBRARY} -DHIREDIS_INCLUDE_DIR=${HIREDIS_INCLUDE_DIR}
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS "redox_master-prefix/src/redox_master-build/libredox_static.a"
)
ExternalProject_Get_Property(redox_master source_dir)
set(REDOX_INCLUDE_DIR ${source_dir}/include/)

ExternalProject_Get_Property(redox_master binary_dir)
set(REDOX_LIBRARIES ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}redox_static.a)
set(REDOX_LIBRARY redox)
add_library(${REDOX_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${REDOX_LIBRARY} PROPERTY IMPORTED_LOCATION ${REDOX_LIBRARIES} )
add_dependencies(${REDOX_LIBRARY} redox_master)
