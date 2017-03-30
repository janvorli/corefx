project(System.Net.Http.Native)

if(FEATURE_LOCAL_CURL)
    set(CURL_INCLUDE_DIR "${THIRD_PARTY_INSTALL_DIRECTORY}/include")
    add_compile_options(-DCURL_STATICLIB)
else()
    find_package(CURL)
    if(NOT CURL_FOUND)
        message(FATAL_ERROR "!!! Cannot find libcurl and System.Net.Http.Native cannot build without it. Try installing libcurl4-openssl-dev (or the appropriate package for your platform) !!!")
    endif(NOT CURL_FOUND)

    if(CMAKE_STATIC_LIB_LINK)
        find_library(CURL_STATIC_LIB NAMES libcurl.a)
        if(NOT CURL_STATIC_LIB)
            message(FATAL_ERROR "!!! Cannot find libcurl static lib and System.Net.Http.Native cannot build without it. Try installing libcurl4-openssl-dev (or the appropriate package for your platform) !!!")
        endif(NOT CURL_STATIC_LIB)
        set(CURL_LIBRARIES ${CURL_STATIC_LIB})
        add_compile_options(-DCURL_STATICLIB)
    endif(CMAKE_STATIC_LIB_LINK)
endif()

set(NATIVEHTTP_SOURCES
    pal_curlinit.cpp
    pal_easy.cpp
    pal_multi.cpp
    pal_slist.cpp
    pal_versioninfo.cpp
)

if(FEATURE_LOCAL_CURL)
    list(APPEND NATIVEHTTP_SOURCES
        curlshim.cpp
    )
endif()

include(configure.cmake)

include_directories(SYSTEM ${CURL_INCLUDE_DIR})

add_library(System.Net.Http.Native
    SHARED
    ${NATIVEHTTP_SOURCES}
    ${VERSION_FILE_PATH}
)

if(FEATURE_LOCAL_CURL)
    add_dependencies(System.Net.Http.Native coreclr_curl)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL FreeBSD AND NOT CMAKE_SYSTEM_NAME STREQUAL NetBSD)
        target_link_libraries(System.Net.Http.Native
            dl
        )
    endif(NOT CMAKE_SYSTEM_NAME STREQUAL FreeBSD AND NOT CMAKE_SYSTEM_NAME STREQUAL NetBSD)
else()
    target_link_libraries(System.Net.Http.Native
        ${CURL_LIBRARIES}
    )
endif()

install_library_and_symbols (System.Net.Http.Native)
