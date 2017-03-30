cmake_minimum_required(VERSION 2.8.12)

include(ExternalProject)

ExternalProject_Add(
    nghttp2
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nghttp2
    CONFIGURE_COMMAND CFLAGS=-O2 CXXFLAGS=-O2 ${CMAKE_CURRENT_SOURCE_DIR}/nghttp2/configure --enable-shared=yes --enable-lib-only=yes --with-pic=yes --prefix=<INSTALL_DIR>
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/nghttp2
    INSTALL_DIR ${THIRD_PARTY_INSTALL_DIRECTORY}
    INSTALL_COMMAND make install
) 

ExternalProject_Add(
    curl
    DEPENDS nghttp2
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/curl
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-gopher --without-librtmp --disable-manual --disable-libcurl-option --with-nghttp2=<INSTALL_DIR> --with-gssapi --enable-ipv6 --with-ssl --with-ca-fallback --without-ca-path --without-ca-bundle --enable-symbol-hiding --enable-optimize --prefix=<INSTALL_DIR>
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/curl
    INSTALL_DIR ${THIRD_PARTY_INSTALL_DIRECTORY}
    INSTALL_COMMAND make install
) 

