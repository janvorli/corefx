project(System.Net.Security.Native)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DPIC=1)

set(NATIVEGSS_SOURCES
    pal_gssapi.cpp
)

add_library(System.Net.Security.Native
    SHARED
    ${NATIVEGSS_SOURCES}
    ${VERSION_FILE_PATH}
)

target_link_libraries(System.Net.Security.Native
    ${LIBGSS}
)

install_library_and_symbols (System.Net.Security.Native)
