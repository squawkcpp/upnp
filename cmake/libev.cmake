# ---------------------             libev               ---------------------
ExternalProject_Add(
  libev_main
  URL "https://github.com/enki/libev/archive/master.zip"
  DOWNLOAD_NO_PROGRESS 1
  CONFIGURE_COMMAND cd <SOURCE_DIR> && ./configure  --enable-static
  BUILD_COMMAND cd <SOURCE_DIR> && make &> ev_build.log
  INSTALL_COMMAND "" #cd <SOURCE_DIR>  && make install 
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS "libev_main-prefix/src/libev_main/.libs/libev.a"
)
ExternalProject_Get_Property(libev_main binary_dir)
ExternalProject_Get_Property(libev_main source_dir)
set(LIBEV_LIBRARY ${source_dir}/.libs/${CMAKE_FIND_LIBRARY_PREFIXES}ev.a)
set(LIBEV_INCLUDE_DIR ${source_dir})
set(LIBEV_LIBRARY_NAME libev)
add_library(${LIBEV_LIBRARY_NAME} UNKNOWN IMPORTED)
set_property(TARGET ${LIBEV_LIBRARY_NAME} PROPERTY IMPORTED_LOCATION ${LIBEV_LIBRARY} )
add_dependencies(${LIBEV_LIBRARY_NAME} libev_main)
