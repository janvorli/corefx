// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
//

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curlshim.h"

// Define pointers to all the used CURL functions
#define PER_FUNCTION_BLOCK(fn) decltype(fn) fn##_ptr;
FOR_ALL_CURL_FUNCTIONS
#undef PER_FUNCTION_BLOCK

static void* libssl = nullptr;
static void* libcurl = nullptr;
static void* libnghttp2 = nullptr;

bool OpenSSLLibrary()
{
    // First try the default versioned so naming as described in the OpenSSL doc
    libssl = dlopen("libssl.so.1.0.0", RTLD_NOW | RTLD_GLOBAL);
    if (libssl == nullptr)
    {
        // Fedora derived distros use different naming for the version 1.0.0
        libssl = dlopen("libssl.so.10", RTLD_NOW | RTLD_GLOBAL);
    }

    return libssl != nullptr;
}

bool OpenNgHttp2Library()
{
    Dl_info dlInfo;
    int st = dladdr(reinterpret_cast<void*>(OpenNgHttp2Library), &dlInfo);
    if (st == 0)
    {
        fprintf(stderr, "\n");
        abort();
    }

    if (dlInfo.dli_fname == nullptr)
    {
        fprintf(stderr, "\n");
        abort();
    }

    const char* lastSlash = strrchr(dlInfo.dli_fname, '/');
    if (lastSlash == nullptr)
    {
        fprintf(stderr, "\n");
        abort();
    }

    size_t pathLength = static_cast<size_t>(lastSlash - dlInfo.dli_fname + 1);
    char* libcurlPath = reinterpret_cast<char*>(malloc(pathLength + sizeof("libnghttp2.so.14") + 1));
    if (libcurlPath == nullptr)
    {
        fprintf(stderr, "\n");
        abort();
    }
    strncpy(libcurlPath, dlInfo.dli_fname, pathLength);
    strcpy(libcurlPath + pathLength, "libnghttp2.so.14");
    libnghttp2 = dlopen(libcurlPath, RTLD_NOW | RTLD_GLOBAL);
    free(libcurlPath);

    return libnghttp2 != nullptr;
}

bool OpenCURLLibrary()
{
    Dl_info dlInfo;
    int st = dladdr(reinterpret_cast<void*>(OpenCURLLibrary), &dlInfo);
    if (st == 0)
    {
        fprintf(stderr, "\n");
        abort();
    }

    if (dlInfo.dli_fname == nullptr)
    {
        fprintf(stderr, "\n");
        abort();
    }

    const char* lastSlash = strrchr(dlInfo.dli_fname, '/');
    if (lastSlash == nullptr)
    {
        fprintf(stderr, "\n");
        abort();
    }

    size_t pathLength = static_cast<size_t>(lastSlash - dlInfo.dli_fname + 1);
    char* libcurlPath = reinterpret_cast<char*>(malloc(pathLength + sizeof("libcurl.so.4") + 1));
    if (libcurlPath == nullptr)
    {
        fprintf(stderr, "\n");
        abort();
    }
    strncpy(libcurlPath, dlInfo.dli_fname, pathLength);
    strcpy(libcurlPath + pathLength, "libcurl.so.4");
    libcurl = dlopen(libcurlPath, RTLD_NOW);
    free(libcurlPath);

    return libcurl != nullptr;
}

__attribute__((constructor))
void InitializeCURLShim()
{
    if (!OpenSSLLibrary())
    {
        fprintf(stderr, "No usable version of the libssl was found\n");
        abort();
    }

    if (!OpenNgHttp2Library())
    {
        fprintf(stderr, "Local libnghttp2.so.14 not found\n");
        abort();
    }

    if (!OpenCURLLibrary())
    {
        fprintf(stderr, "Local libcurl.so.4 not found\n");
        abort();
    }

    // Get pointers to all the CURL functions that are needed
#define PER_FUNCTION_BLOCK(fn) \
    fn##_ptr = reinterpret_cast<decltype(fn)>(dlsym(libcurl, #fn)); \
    if ((fn##_ptr) == NULL) { fprintf(stderr, "Cannot get required symbol " #fn " from libcurl\n"); abort(); }

    FOR_ALL_CURL_FUNCTIONS
#undef PER_FUNCTION_BLOCK    
}

__attribute__((destructor))
void ShutdownCURLShim()
{
    if (libcurl != nullptr)
    {
        dlclose(libcurl);
    }

    if (libnghttp2 != nullptr)
    {
        dlclose(libnghttp2);
    }

    if (libssl != nullptr)
    {
        dlclose(libssl);
    }
}
