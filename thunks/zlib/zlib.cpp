#include <filesystem>
#include <stdlib.h>
#include <zlib.h>
#include <zip.h>

#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "thunk_gen.h"
#include "so_util.h"
#include "gmloader/configuration.h"

namespace fs = std::filesystem;

typedef struct AAssetManager_impl{
    zip_t *apk;
} AAssetManager_impl;

typedef struct AAsset_impl{
    AAssetManager_impl *mgr;
    zip_int64_t index;
    zip_file_t *file;
    int mode;
    off_t current_offset;
    bool persistent_read;
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
        fatal_error("[AAsset] Cannot open zip archive '%s': %s\n", gmloader_config.apk_path.c_str(), zip_error_strerror(&error));
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
    asset->persistent_read = false;
    asset->file = NULL;
    asset->current_offset = 0;

    if(!asset_mgr->apk)
    {
        fatal_error("[AAsset] APK not initialized.\n");
        return NULL;
    }

    fs::path filename_restored = fs::path("assets") / filename;
    if ((asset->index = zip_name_locate(asset->mgr->apk, filename_restored.c_str(), ZIP_FL_NOCASE)) == -1)
    {
        fatal_error("[AAsset] Cannot locate %s in AAsset archive.\n", filename_restored.c_str());
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
        fatal_error("[AAsset] APK not initialized.\n");
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

    // Games that don't use seek need to reopen the file for every read
    if (!asset->persistent_read || !asset->file) {

        if((asset->file = zip_fopen_index(asset->mgr->apk, asset->index, ZIP_FL_UNCHANGED)) == NULL)
        {
            fatal_error("[AAsset] Cannot open file (index = %ld).\n", asset->index);
            return 0;
        }
    }

    if((nread=zip_fread(asset->file, buf, count)) == -1)
    {
        fatal_error("[AAsset] Cannot read file (index = %ld).\n", asset->index);
        return 0;
    }

    asset->current_offset += nread;

    if (!asset->persistent_read && nread < count)
    {
        zip_fclose(asset->file);
        asset->file = NULL;
    }

    return nread;
}

ABI_ATTR void AAsset_close_impl(void *f)
{
    AAsset_impl* asset = (AAsset_impl*)f;
    if (asset) {
        if (asset->file) {
            zip_fclose(asset->file);
            asset->file = NULL;
        }
        free(asset);
    }
}

ABI_ATTR off_t AAsset_seek_impl(void* asset_ptr, off_t offset, int whence)
{
    AAsset_impl* asset = (AAsset_impl*)asset_ptr;
    asset->persistent_read = true;
    
    if (!asset || !asset->file) {
        fatal_error("[AAsset] Cannot seek NULL asset.\n");
        return -1;
    }

    if (zip_file_is_seekable(asset->file))
    {
        if (zip_fseek(asset->file, offset, whence) == -1)
        {
            warning("[AAsset] Failed to seek seekable file!\n");
        }
        
        return zip_ftell(asset->file);
    }
    else
    {
        // Get file size for validation
        zip_stat_t stt = {};
        zip_stat_init(&stt);
        if (zip_stat_index(asset->mgr->apk, asset->index, ZIP_STAT_SIZE, &stt) == -1) {
            fatal_error("[AAsset] Cannot get file size for index %ld.\n", asset->index);
            return -1;
        }

        off_t file_size = stt.size;

        // Calculate new offset
        off_t new_offset;
        switch (whence) {
            case SEEK_SET:
                new_offset = offset;
                break;
            case SEEK_CUR:
                new_offset = asset->current_offset + offset;
                break;
            case SEEK_END:
                new_offset = file_size + offset;
                break;
            default:
                warning("[AAsset] invalid whence value %d\n", whence);
                return -1;
        }

        // Validate new offset
        if (new_offset < 0 || new_offset > file_size) {
            fatal_error("[AAsset] Seek offset %ld out of bounds (size=%ld)\n", new_offset, file_size);
            return -1;
        }

        // libzip doesn't support direct seeking, so we need to:
        // 1. Close the current file handle
        // 2. Reopen and read/skip to the desired position
        if (asset->file) {
            zip_fclose(asset->file);
        }

        // Reopen the file
        asset->file = zip_fopen_index(asset->mgr->apk, asset->index, ZIP_FL_UNCHANGED);
        if (!asset->file) {
            fatal_error("Cannot reopen file for seeking (index = %ld)\n", asset->index);
            return -1;
        }

        // If not seeking to start, skip bytes to reach target position
        if (new_offset > 0) {
            char buffer[4096];
            off_t remaining = new_offset;
            
            while (remaining > 0) {
                size_t to_read = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
                zip_int64_t nread = zip_fread(asset->file, buffer, to_read);
                
                if (nread <= 0) {
                    fatal_error("[AAsset] Failed to seek to position %ld\n", new_offset);
                    zip_fclose(asset->file);
                    asset->file = NULL;
                    return -1;
                }
                remaining -= nread;
            }
        }

        // Update the tracked offset and return it
        asset->current_offset = new_offset;
        return new_offset;
    }
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
    NO_THUNK("AAsset_seek", (uintptr_t)&AAsset_seek_impl),
    
    {NULL, (uintptr_t)NULL}
};
