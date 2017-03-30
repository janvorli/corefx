include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_INCLUDES ${CURL_INCLUDES})
set(CMAKE_REQUIRED_LIBRARIES ${CURL_LIBRARY})

check_cxx_source_compiles(
    "
    #include <curl/multi.h>
    int main() { int i = CURLM_ADDED_ALREADY; }
    "
    HAVE_CURLM_ADDED_ALREADY)

check_cxx_source_compiles(
    "
    #include <curl/multi.h>
    int main() { int i = CURL_HTTP_VERSION_2_0; }
    "
    HAVE_CURL_HTTP_VERSION_2_0)

check_cxx_source_compiles(
    "
    #include <curl/multi.h>
    int main() { int i = CURLPIPE_MULTIPLEX; }
    "
    HAVE_CURLPIPE_MULTIPLEX)

check_cxx_source_compiles(
    "
    #include <curl/curl.h>
    int main()
    {
        int i = CURL_SSLVERSION_TLSv1_0;
        i = CURL_SSLVERSION_TLSv1_1;
        i = CURL_SSLVERSION_TLSv1_2;
    }
    "
    HAVE_CURL_SSLVERSION_TLSv1_012)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pal_curl_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/pal_curl_config.h)
