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
static void* libcoreclr_curl = nullptr;

#define LIBCORECLR_CURL_FILENAME "libcoreclr_curl.so"

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

bool OpenCoreCLRCURLLibrary()
{
    Dl_info dlInfo;
    int st = dladdr(reinterpret_cast<void*>(OpenCoreCLRCURLLibrary), &dlInfo);
    if (st == 0)
    {
        return false;
    }

    if (dlInfo.dli_fname == nullptr)
    {
        return false;
    }

    const char* lastSlash = strrchr(dlInfo.dli_fname, '/');
    if (lastSlash == nullptr)
    {
        return false;
    }

    size_t pathLength = static_cast<size_t>(lastSlash - dlInfo.dli_fname + 1);
    char* localLibraryPath = reinterpret_cast<char*>(malloc(pathLength + sizeof(LIBCORECLR_CURL_FILENAME)));    

    if (localLibraryPath == nullptr)
    {
        return false;
    }

    strncpy(localLibraryPath, dlInfo.dli_fname, pathLength);
    char* libPath = localLibraryPath;

    strcpy(libPath + pathLength, LIBCORECLR_CURL_FILENAME);
    libcoreclr_curl = dlopen(libPath, RTLD_NOW);

    free(libPath);

    return libcoreclr_curl != nullptr;
}

__attribute__((constructor))
void InitializeCURLShim()
{
    // We need to preload libssl and libnghttp2 before opening libcurl to ensure that
    // it will get the right versions

    // TODO: we should not preload the openssl library in case of non-portable build
    // We should rather make the libcoreclr_curl.so explicitly depend on libssl in that case

    if (!OpenSSLLibrary())
    {
        fprintf(stderr, "No usable version of the libssl was found\n");
        abort();
    }

    if (!OpenCoreCLRCURLLibrary())
    {
        fprintf(stderr, "Local " LIBCORECLR_CURL_FILENAME " not found\n");
        abort();
    }

    // Get pointers to all the CURL functions that are needed
#define PER_FUNCTION_BLOCK(fn) \
    fn##_ptr = reinterpret_cast<decltype(fn)>(dlsym(libcoreclr_curl, #fn)); \
    if ((fn##_ptr) == NULL) { fprintf(stderr, "Cannot get required symbol " #fn " from libcoreclr_curl\n"); abort(); }

    FOR_ALL_CURL_FUNCTIONS
#undef PER_FUNCTION_BLOCK    
}

__attribute__((destructor))
void ShutdownCURLShim()
{
    if (libcoreclr_curl != nullptr)
    {
        dlclose(libcoreclr_curl);
    }

    if (libssl != nullptr)
    {
        dlclose(libssl);
    }
}
