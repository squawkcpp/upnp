include(ExternalProject)
EXTERNALPROJECT_ADD(
    openssl
    URL http://www.openssl.org/source/openssl-1.0.2-latest.tar.gz
    PATCH_COMMAND "" #${CMAKE_SOURCE_DIR}/patches/patch-manager.sh openssl
    CONFIGURE_COMMAND ./config no-shared --prefix=${CMAKE_BINARY_DIR}
    BUILD_COMMAND make depend && make
    INSTALL_COMMAND "" #make install_sw
    BUILD_IN_SOURCE 1
    BUILD_BYPRODUCTS openssl-prefix/src/openssl/libssl.a
    BUILD_BYPRODUCTS openssl-prefix/src/openssl/libcrypto.a
)
ExternalProject_Get_Property(openssl source_dir)
set(OPENSSL_INCLUDE_DIR ${source_dir}/include/)
ExternalProject_Get_Property(openssl binary_dir)
set(OPENSSL_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}ssl.a)
set(CRYPTO_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}crypto.a)

set(OPENSSL_LIBRARY ssl)
add_library(${OPENSSL_LIBRARY} STATIC IMPORTED)
set_property(TARGET ${OPENSSL_LIBRARY} PROPERTY IMPORTED_LOCATION ${OPENSSL_LIBRARY_PATH} )
add_dependencies(${OPENSSL_LIBRARY} openssl)

set(CRYPTO_LIBRARY crypto)
add_library(${CRYPTO_LIBRARY} STATIC IMPORTED)
set_property(TARGET ${CRYPTO_LIBRARY} PROPERTY IMPORTED_LOCATION ${CRYPTO_LIBRARY_PATH} )
add_dependencies(${CRYPTO_LIBRARY} openssl)

