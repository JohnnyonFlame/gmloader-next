#include <stdlib.h>
#include <zlib.h>
#include <zip.h>

#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "thunk_gen.h"
#include "so_util.h"
#include "gmloader/configuration.h"

typedef struct AAssetManager_impl{
    zip_t *apk;
} AAssetManager_impl;

typedef struct AAsset_impl{
    AAssetManager_impl *mgr;
    zip_int64_t index;
    zip_file_t *file;
    int mode;
} AAsset_impl;

/*
Original code of the function can be found here:
https://android.googlesource.com/platform/frameworks/base/+/master/native/android/asset_manager.cpp
*/

ABI_ATTR void *AAssetManager_fromJava_impl(void *env, void *obj)
{
    int err;
    AAssetManager_impl * asset_mgr = (AAssetManager_impl*)malloc(sizeof(AAssetManager_impl));

    if ((asset_mgr->apk = zip_open(gmloader_config.apk_path.c_str(), ZIP_RDONLY, &err)) == NULL)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        fprintf(stderr, "%s: cannot open zip archive '%s': %s\n",
            __func__, gmloader_config.apk_path.c_str(), zip_error_strerror(&error));
        zip_error_fini(&error);
    }

    if (!asset_mgr->apk)
    {
        return NULL;
    }
    else
    {
        return (void*)asset_mgr;
    }
}

ABI_ATTR void *AAssetManager_open_impl(void *mgr, const char *filename, int mode)
{

    /* We don't care about AccessMode for now, but for future reference:
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

    AAssetManager_impl* asset_mgr = (AAssetManager_impl*) mgr;
    AAsset_impl* asset = (AAsset_impl*)malloc(sizeof(AAsset_impl));
    asset->mgr = asset_mgr;
    asset->mode = mode;

    if(!asset_mgr->apk)
    {
        fprintf(stderr, "%s: apk not initialized\n",__func__);
        return NULL;
    }

    char filename_restored[strlen(filename) + 7] = "assets/";
    strcat(filename_restored, filename);

    if ((asset->index = zip_name_locate(asset->mgr->apk, filename_restored, ZIP_FL_NOCASE)) == -1)
    {
        fprintf(stderr, "%s: cannot locate %s\n",__func__, filename_restored);
        return NULL;
    }
    else
    {
        return (void*)asset;
    }

}

ABI_ATTR off_t AAsset_getLength_impl(void *f)
{
    AAsset_impl* asset = (AAsset_impl*)f;
    if(!asset)
    {
        fprintf(stderr, "%s: asset not initialized\n",__func__);
        return 0;
    }

    zip_stat_t stt = {};
    zip_stat_init(&stt);
    if ((zip_stat_index(asset->mgr->apk, asset->index, ZIP_STAT_SIZE, &stt)) == -1)
    {
        return 0;
    }

    return stt.size;

}

ABI_ATTR off_t AAsset_read_impl(void *f, void *buf, size_t count)
{
    AAsset_impl* asset = (AAsset_impl*)f;
    int nread;
    if((asset->file = zip_fopen_index(asset->mgr->apk, asset->index, ZIP_FL_UNCHANGED)) == NULL)
    {
        fprintf(stderr, "%s: cannot open the file (index = %ld)\n",__func__, asset->index);
        return 0;
    }

    if((nread=zip_fread(asset->file, buf, count)) == -1)
    {
        fprintf(stderr, "%s: cannot read the file\n",__func__);
        return 0;
    }
    
    return nread;
}

ABI_ATTR void AAsset_close_impl(void *f)
{
    AAsset_impl* asset = (AAsset_impl*)f;
    free(asset->file);
    free(asset);
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
