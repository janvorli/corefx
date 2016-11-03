#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "shim.h"
static void* libcrypto = nullptr;
static void* libssl = nullptr;
// Define pointers to all the used libcrypto / libssl functions
//#define PER_FUNCTION_BLOCK(fn, lib) decltype(fn)* fn##_ptr;
#define PER_FUNCTION_BLOCK(fn, lib) decltype(fn) fn;
FOR_ALL_FUNCTIONS
#undef PER_FUNCTION_BLOCK

bool OpenLibraries(const char* version)
{
    char libcryptoName[64];
    char libsslName[64];

    sprintf(libcryptoName, "libcrypto.so.%s", version);
    sprintf(libsslName, "libssl.so.%s", version);

    libcrypto = dlopen(libcryptoName, RTLD_LAZY);
    if (libcrypto != nullptr)
    {
        libssl = dlopen(libsslName, RTLD_LAZY);
        if (libssl == nullptr)
        {
            dlclose(libcrypto);
            libcrypto = nullptr;
        }
    }

    return libcrypto != nullptr;
}

__attribute__((constructor))
void InitializeShim()
{
    if (!OpenLibraries("10") &&
        !OpenLibraries("1.0.0"))
    {
        // No usable ssl / crypto libraries found
        fprintf(stderr, "No usable version of the libssl and libcrypto was found\n");
        abort();
    }
    // Get pointers to all the ICU functions that are needed
#define PER_FUNCTION_BLOCK(fn, lib) \
    fn##_ptr = reinterpret_cast<decltype(fn)>(dlsym(lib, #fn)); \
    if (fn##_ptr == NULL) { fprintf(stderr, "Cannot get symbol " #fn "from " #lib "\n"); abort(); }

    FOR_ALL_FUNCTIONS
#undef PER_FUNCTION_BLOCK
}

__attribute__((destructor))
void ShutdownShim()
{
    if (libcrypto != nullptr)
    {
        dlclose(libcrypto);
    }
   
    if (libssl != nullptr)
    {
        dlclose(libssl);
    }
}

