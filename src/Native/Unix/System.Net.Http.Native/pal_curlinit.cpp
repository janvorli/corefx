// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "pal_config.h"
#include "pal_curlinit.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <curl/curl.h>

#ifdef FEATURE_STANDALONE_PACKAGE
CAStorage caStorage = CAStorage::Unknown;

static int32_t DetectCAStorageLocation()
{
    int32_t errorCode = CURLE_OK;
    struct stat caLocationStat;

    int st = stat("/etc/pki/tls/certs/ca-bundle.crt", &caLocationStat);
    if (st == 0)
    {
        caStorage = CAStorage::BundleFile;
    }
    else if (st == ENOENT)
    {
        st = stat("/etc/ssl/certs", &caLocationStat);
        if (st == 0)
        {
            caStorage = CAStorage::Directory;
        }
        else
        {
            errorCode = CURLE_FAILED_INIT;
        }
    }
    else
    {
        errorCode = CURLE_FAILED_INIT;
    }

    return errorCode;
}
#endif // FEATURE_STANDALONE_PACKAGE

extern "C" int32_t HttpNative_EnsureCurlIsInitialized()
{
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    static bool initializationAttempted = false;
    static int32_t errorCode = -1;

    pthread_mutex_lock(&lock);
    {
        if (!initializationAttempted)
        {
            errorCode = curl_global_init(CURL_GLOBAL_ALL);

#ifdef FEATURE_STANDALONE_PACKAGE
            if (errorCode == CURLE_OK)
            {
                errorCode = DetectCAStorageLocation();
            }
#endif // FEATURE_STANDALONE_PACKAGE

            initializationAttempted = true;
        }
    }
    pthread_mutex_unlock(&lock);

    return errorCode;
}
