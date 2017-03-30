project(coreclr_curl)
include(FindZLIB)

set(CMAKE_SHARED_LIBRARY_PREFIX "lib")

set(CURL_LIBRARY ${THIRD_PARTY_INSTALL_DIRECTORY}/lib/libcurl.a)
set(NGHTTP2_LIBRARY ${THIRD_PARTY_INSTALL_DIRECTORY}/lib/libnghttp2.a)

set(START_WHOLE_ARCHIVE -Wl,--whole-archive)
set(END_WHOLE_ARCHIVE -Wl,--no-whole-archive) 
set(EXPORTS_LINKER_OPTION -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/coreclr_curl.exports)

add_library(coreclr_curl
    SHARED
    dummy.cpp
)

add_dependencies(coreclr_curl curl nghttp2)

target_link_libraries(
    coreclr_curl
    ${START_WHOLE_ARCHIVE}
    ${CURL_LIBRARY}
    ${END_WHOLE_ARCHIVE}
    ${NGHTTP2_LIBRARY}
    ${ZLIB_LIBRARIES}
    ${LIBGSS}
)

install_library_and_symbols (coreclr_curl)
