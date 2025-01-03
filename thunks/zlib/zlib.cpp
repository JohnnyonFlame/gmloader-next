#include <stdlib.h>
#include <zlib.h>
#include <zip.h>

#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "thunk_gen.h"
#include "so_util.h"
#include "gmloader/configuration.h"

zip_t *apk;
zip_file_t *asset_file;
zip_int64_t asset_index;

extern gmloader::config gmloader_config;


/*
https://android.googlesource.com/platform/frameworks/base/+/master/native/android/asset_manager.cpp
*/

ABI_ATTR void *AAssetManager_fromJava_impl(void *env, void *obj)
{
    int err;

    if (!apk)
    {
        if ((apk = zip_open(gmloader_config.apk_path.c_str(), ZIP_RDONLY, &err)) == NULL)
        {
            zip_error_t error;
            zip_error_init_with_code(&error, err);
            fprintf(stderr, "%s: cannot open zip archive '%s': %s\n",
                __func__, gmloader_config.apk_path.c_str(), zip_error_strerror(&error));
            zip_error_fini(&error);
        }
    }

    return (void*)apk;
}

ABI_ATTR void *AAssetManager_open_impl(void *mgr, const char *filename, int mode)
{

    /* we don't care for now about the mode but we never now:
    https://android.googlesource.com/platform/frameworks/base/+/master/libs/androidfw/include/androidfw/Asset.h
    typedef enum AccessMode {
        ACCESS_UNKNOWN = 0,
        // read chunks, and seek forward and backward
        ACCESS_RANDOM,
        // read sequentially, with an occasional forward seek
        ACCESS_STREAMING,
        // caller plans to ask for a read-only buffer with all data
        ACCESS_BUFFER,
    } AccessMode;
    */

    if(!apk)
    {
        fprintf(stderr, "%s: apk not initialized\n",__func__);
        return NULL;
    }

    if ((asset_index = zip_name_locate(apk, filename, ZIP_FL_NOCASE)) == -1)
    {
        fprintf(stderr, "%s: cannot locate %s\n",__func__, filename);
    }
    else
    {
        return (void*)asset_index;
    }

    /*
        No safety test done here, if filename length > 1024 - 7
        gmloadernext will crash with:

        *** buffer overflow detected ***: terminated

        TODO: fix this :)
    */
    char filename_restored[1024] = "assets/";
    strcat(filename_restored, filename);

    if ((asset_index = zip_name_locate(apk, filename_restored, ZIP_FL_NOCASE)) == -1)
    {
        fprintf(stderr, "%s: cannot locate %s\n",__func__, filename_restored);
        return NULL;
    }
    else
    {
        return (void*)asset_index;
    }

}

ABI_ATTR off_t AAsset_getLength_impl()
{
    if(!apk || !asset_index)
    {
        fprintf(stderr, "%s: apk or asset not initialized\n",__func__);
        return 0;
    }

    zip_stat_t stt = {};
    zip_stat_init(&stt);
    if ((zip_stat_index(apk, asset_index, ZIP_STAT_SIZE, &stt)) == -1)
    {
        return 0;
    }

    return stt.size;

}

ABI_ATTR off_t AAsset_read_impl(void *f, void *buf, size_t count)
{
    int nread;
    if((asset_file = zip_fopen_index(apk, asset_index, ZIP_FL_UNCHANGED)) == NULL)
    {
        fprintf(stderr, "%s: cannot open the file (index = %ld)\n",__func__, asset_index);
        return 0;
    }


    if((nread=zip_fread(asset_file, buf, count)) == -1)
    {
        fprintf(stderr, "%s: cannot read the file\n",__func__);
        return 0;
    }
    
    return nread;
}

ABI_ATTR void AAsset_close_impl(void *f)
{
    asset_index = 0;
    asset_index = -1;
}

DynLibFunction symtable_zlib[] = {
    THUNK_DIRECT(inflate),
    THUNK_DIRECT(inflateInit_),
    THUNK_DIRECT(inflateInit2_),
    THUNK_DIRECT(inflateEnd),
    THUNK_DIRECT(inflateReset),
    THUNK_DIRECT(deflate),
    THUNK_DIRECT(compress),
    THUNK_DIRECT(compress2),
    THUNK_DIRECT(deflateEnd),
    THUNK_DIRECT(deflateInit_),
    THUNK_DIRECT(deflateInit2_),
    THUNK_DIRECT(deflateReset),
    THUNK_DIRECT(crc32),

    NO_THUNK("AAssetManager_fromJava", (uintptr_t)&AAssetManager_fromJava_impl),
    NO_THUNK("AAssetManager_open", (uintptr_t)&AAssetManager_open_impl),
    NO_THUNK("AAsset_getLength", (uintptr_t)&AAsset_getLength_impl),
    NO_THUNK("AAsset_read", (uintptr_t)&AAsset_read_impl),
    NO_THUNK("AAsset_close", (uintptr_t)&AAsset_close_impl),
    
    {NULL, (uintptr_t)NULL}
};