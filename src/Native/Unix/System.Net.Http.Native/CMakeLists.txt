project(System.Net.Http.Native)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
if(CMAKE_STATIC_LIB_LINK)
    set(CURL_LIBRARIES ${CMAKE_INSTALL_PREFIX}/ThirdParty/lib/libcurl.a)
    add_compile_options(-DCURL_STATICLIB)
else()
    set(CURL_LIBRARIES ${CMAKE_INSTALL_PREFIX}/ThirdParty/lib/libcurl.so)
endif(CMAKE_STATIC_LIB_LINK)

set(NATIVEHTTP_SOURCES
    pal_curlinit.cpp
    pal_easy.cpp
    pal_multi.cpp
    pal_slist.cpp
    pal_versioninfo.cpp
)

include_directories(SYSTEM "${CMAKE_INSTALL_PREFIX}/ThirdParty/include")

add_library(System.Net.Http.Native
    SHARED
    ${NATIVEHTTP_SOURCES}
    ${VERSION_FILE_PATH}
)

add_dependencies(System.Net.Http.Native curl)

target_link_libraries(System.Net.Http.Native
  ${CURL_LIBRARIES}
)

install_library_and_symbols (System.Net.Http.Native)
