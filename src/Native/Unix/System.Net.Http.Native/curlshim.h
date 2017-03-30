// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
//

// Enable calling CURL functions through shims to enable us to preload
// libssl before the libcurl

#pragma once

// All the curl includes need to be here to ensure that the APIs we use
// are overriden to be called through our function pointers.
#include <curl/curl.h>

// List of all functions from the libcurl that are used in the System.Net.Http.Native.
// Forgetting to add a function here results in build failure with message reporting the function
// that needs to be added.
#define FOR_ALL_CURL_FUNCTIONS \
    PER_FUNCTION_BLOCK(curl_easy_cleanup) \
    PER_FUNCTION_BLOCK(curl_easy_getinfo) \
    PER_FUNCTION_BLOCK(curl_easy_init) \
    PER_FUNCTION_BLOCK(curl_easy_pause) \
    PER_FUNCTION_BLOCK(curl_easy_perform) \
    PER_FUNCTION_BLOCK(curl_easy_setopt) \
    PER_FUNCTION_BLOCK(curl_easy_strerror) \
    PER_FUNCTION_BLOCK(curl_global_init) \
    PER_FUNCTION_BLOCK(curl_multi_add_handle) \
    PER_FUNCTION_BLOCK(curl_multi_cleanup) \
    PER_FUNCTION_BLOCK(curl_multi_info_read) \
    PER_FUNCTION_BLOCK(curl_multi_init) \
    PER_FUNCTION_BLOCK(curl_multi_perform) \
    PER_FUNCTION_BLOCK(curl_multi_remove_handle) \
    PER_FUNCTION_BLOCK(curl_multi_setopt) \
    PER_FUNCTION_BLOCK(curl_multi_strerror) \
    PER_FUNCTION_BLOCK(curl_multi_wait) \
    PER_FUNCTION_BLOCK(curl_slist_append) \
    PER_FUNCTION_BLOCK(curl_slist_free_all) \
    PER_FUNCTION_BLOCK(curl_version_info) \

#undef curl_easy_setopt
#undef curl_easy_getinfo
#undef curl_share_setopt
#undef curl_multi_setopt

// Declare pointers to all the used CURL functions
#define PER_FUNCTION_BLOCK(fn) extern decltype(fn)* fn##_ptr;
FOR_ALL_CURL_FUNCTIONS
#undef PER_FUNCTION_BLOCK

// Redefine all calls to OpenSSL functions as calls through pointers that are set
// to the functions from the libssl.so selected by the shim.
#define curl_easy_cleanup curl_easy_cleanup_ptr
#define curl_easy_getinfo curl_easy_getinfo_ptr
#define curl_easy_init curl_easy_init_ptr
#define curl_easy_pause curl_easy_pause_ptr
#define curl_easy_perform curl_easy_perform_ptr
#define curl_easy_setopt curl_easy_setopt_ptr
#define curl_easy_strerror curl_easy_strerror_ptr
#define curl_global_init curl_global_init_ptr
#define curl_multi_add_handle curl_multi_add_handle_ptr
#define curl_multi_cleanup curl_multi_cleanup_ptr
#define curl_multi_info_read curl_multi_info_read_ptr
#define curl_multi_init curl_multi_init_ptr
#define curl_multi_perform curl_multi_perform_ptr
#define curl_multi_remove_handle curl_multi_remove_handle_ptr
#define curl_multi_setopt curl_multi_setopt_ptr
#define curl_multi_strerror curl_multi_strerror_ptr
#define curl_multi_wait curl_multi_wait_ptr
#define curl_slist_append curl_slist_append_ptr
#define curl_slist_free_all curl_slist_free_all_ptr
#define curl_version_info curl_version_info_ptr
