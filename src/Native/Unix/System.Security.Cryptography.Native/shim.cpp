// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
//

// Enable calling openssl functions through shims to allow using the 
// same System.Security.Cryptography.Native.so on all glibc based
// Linux distros.

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "shim.h"

static void* libssl = nullptr;

// Define pointers to all the used libssl functions
#define PER_FUNCTION_BLOCK(fn, optional) decltype(fn) fn;
FOR_ALL_FUNCTIONS
#undef PER_FUNCTION_BLOCK

__attribute__((constructor))
void InitializeShim()
{
    libssl = dlopen("libssl.so.1.0.0", RTLD_LAZY);
    if (libssl == nullptr)
    {
        // Fedora derived distros uses version 10 to represent 1.0.0
        libssl = dlopen("libssl.so.10", RTLD_LAZY);
        if (libssl == nullptr)
        {
            // No usable libssl found
            fprintf(stderr, "No usable version of the libssl found\n");
            abort();
        }
    }

    // Get pointers to all the libssl functions that are needed
#define PER_FUNCTION_BLOCK(fn, optional) \
    fn##_ptr = reinterpret_cast<decltype(fn)>(dlsym(libssl, #fn)); \
    if ((fn##_ptr == NULL) && !optional) { fprintf(stderr, "Cannot get symbol " #fn " from libssl\n"); abort(); }

    FOR_ALL_FUNCTIONS
#undef PER_FUNCTION_BLOCK
}

__attribute__((destructor))
void ShutdownShim()
{
    if (libssl != nullptr)
    {
        dlclose(libssl);
    }
}

