include(CheckCXXSourceCompiles)

if(FEATURE_LOCAL_CURL)
    set(HAVE_CURLM_ADDED_ALREADY 1)
    set(HAVE_CURLPIPE_MULTIPLEX 1)
    set(HAVE_CURL_HTTP_VERSION_2_0 1)
    set(HAVE_CURL_SSLVERSION_TLSv1_012 1)
else()

    set(CMAKE_REQUIRED_INCLUDES ${CURL_INCLUDE_DIR})
    set(CMAKE_REQUIRED_LIBRARIES ${CURL_LIBRARIES})

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
        
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pal_curl_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/pal_curl_config.h)
