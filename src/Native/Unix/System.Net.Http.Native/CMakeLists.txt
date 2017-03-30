project(System.Net.Http.Native)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")

set(THIRD_PARTY_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX}/ThirdParty)

if(CMAKE_STATIC_LIB_LINK)
    set(CURL_LIBRARY ${THIRD_PARTY_INSTALL_DIRECTORY}/lib/libcurl.a)
    add_compile_options(-DCURL_STATICLIB)
else()
    set(CURL_LIBRARY ${THIRD_PARTY_INSTALL_DIRECTORY}/lib/libcurl.so)
endif(CMAKE_STATIC_LIB_LINK)

set(NATIVEHTTP_SOURCES
    curlshim.cpp
    pal_curlinit.cpp
    pal_easy.cpp
    pal_multi.cpp
    pal_slist.cpp
    pal_versioninfo.cpp
)

set(CURL_INCLUDES "${THIRD_PARTY_INSTALL_DIRECTORY}/include")

include(configure.cmake)

include_directories(SYSTEM ${CURL_INCLUDES})

add_library(System.Net.Http.Native
    SHARED
    ${NATIVEHTTP_SOURCES}
    ${VERSION_FILE_PATH}
)

add_dependencies(System.Net.Http.Native curl)

if(NOT CMAKE_SYSTEM_NAME STREQUAL FreeBSD AND NOT CMAKE_SYSTEM_NAME STREQUAL NetBSD)
target_link_libraries(System.Net.Http.Native
    dl
)
endif(NOT CMAKE_SYSTEM_NAME STREQUAL FreeBSD AND NOT CMAKE_SYSTEM_NAME STREQUAL NetBSD)

install_library_and_symbols (System.Net.Http.Native)
