#include <stdlib.h>
#include <zlib.h>
#include <zip.h>

#include "platform.h"
#include "thunk_gen.h"
#include "so_util.h"

ABI_ATTR void *AAssetManager_fromJava_impl(void *env, void *obj)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR void *AAssetManager_open_impl(void *mgr, const char *filename, int mode)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR off_t AAsset_getLength_impl(void *f)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR off_t AAsset_read_impl(void *f, void *buf, size_t count)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR void AAsset_close_impl(void *f)
{
    WARN_STUB;
    /* */
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