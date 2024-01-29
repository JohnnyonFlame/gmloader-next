#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdarg.h>
#include "platform.h"
#include "so_util.h"

#include "bionic_file.h"

extern "C" ABI_ATTR int login_tty_impl(int fd)
{
    return -1;
}

extern "C" ABI_ATTR long syscall_impl(long number, ...)
{
    if (number == 0xb2)
        return gettid();
    
    return 0;
}

extern "C" ABI_ATTR void abort_impl(void)
{
    fatal_error("Guest called abort!\n");
    exit(-1);
}

extern "C" ABI_ATTR void *dlopen_impl(const char *filename, int flags)
{
    if (filename == NULL)
        return NULL;

    char *fn = strdup(filename);
    char *ex = basename(fn);
    int ret = strncmp(ex, "libEGL", 6) == 0 ||
              strncmp(ex, "libGL", 5) == 0;

    return (ret) ? (void*)0xDEAD : NULL;
}

extern "C" ABI_ATTR char *dlerror_impl(void)
{
    WARN_STUB
    return NULL;
}

extern "C" ABI_ATTR int dlclose_impl(void *handle)
{
    /* ... */
    return 0;
}

extern "C" ABI_ATTR void *dlsym_impl(void *handle, const char *name)
{
    return (void*)so_resolve_link(NULL, name);
}

extern "C" ABI_ATTR const void *
memchr_impl (const void *__s, int __c, size_t __n)
{
  return __builtin_memchr (__s, __c, __n);
}

extern "C" ABI_ATTR int sigsetmask_impl(int mask)
{
    WARN_STUB
    return -1;
}

extern "C" ABI_ATTR char *tempnam_impl(const char *dir, const char *pfx)
{
    WARN_STUB
    return NULL;
}

extern "C" ABI_ATTR char *tmpnam_impl(char *s)
{
    WARN_STUB
    return NULL;
}

extern "C" ABI_ATTR char *mktemp_impl(char *_template)
{
    WARN_STUB
    return NULL;
}

extern "C" ABI_ATTR int* __errno_impl(void)
{
    return __errno_location();
}

extern "C" ABI_ATTR int __android_log_print_impl(int prio, const char *tag, const char *fmt, ...)
{
    char andlog[2048] = {};
    va_list va;
    va_start(va, fmt);
    warning("LOG[%s]: ", tag);
    int r = vsnprintf(andlog, 2047, fmt, va);
    warning("%s", andlog);
    va_end(va);
    return r;
}

extern "C" ABI_ATTR int __android_log_vprint_impl(int prio, const char *tag, const char *fmt, va_list va)
{
    char andlog[2048] = {};
    warning("LOG[%s]: ", tag);
    int r = vsnprintf(andlog, 2047, fmt, va);
    warning("%s", andlog);
    return r;
}

extern "C" ABI_ATTR const char* __strchr_chk(const char* __s, int __ch, size_t __n) { return strchr(__s, __ch); }
extern "C" ABI_ATTR const char* __strrchr_chk(const char* __s, int __ch, size_t __n) { return strrchr(__s, __ch); }
extern "C" ABI_ATTR size_t __strlen_chk(const char* __s, size_t __n) { return strnlen(__s, __n); }

extern "C" ABI_ATTR void android_set_abort_message_impl(const char* msg)
{
    fatal_error("%s", msg);
    abort();
}

extern "C" ABI_ATTR int __system_property_get_impl(const char *name, char *value)
{
    WARN_STUB;
    value[0] = 0;
    return 0;
}

extern "C" ABI_ATTR int __open_2_impl(const char* pathname, int flags) {
  return open(pathname, flags);
}

// Taken from https://github.com/libhybris/libhybris/blob/master/hybris/common/hooks.c
ABI_ATTR int scandirat_impl(int fd, const char *dir,
                      struct bionic_dirent ***namelist,
                      int (*filter) (const struct bionic_dirent *),
                      int (*compar) (const struct bionic_dirent **,
                                     const struct bionic_dirent **))
{
    struct dirent **namelist_r;
    struct bionic_dirent **result;
    struct bionic_dirent *filter_r;

    int i = 0;
    size_t nItems = 0;

    int res = scandirat(fd, dir, &namelist_r, NULL, NULL);

    if (res > 0 && namelist_r != NULL) {
        result = (bionic_dirent**)malloc(res * sizeof(struct bionic_dirent));
        if (!result)
            return -1;

        for (i = 0; i < res; i++) {
            filter_r = (bionic_dirent*)malloc(sizeof(struct bionic_dirent));
            if (!filter_r) {
                while (i-- > 0)
                    free(result[i]);
                free(result);
                return -1;
            }

            filter_r->d_ino = namelist_r[i]->d_ino;
            filter_r->d_off = namelist_r[i]->d_off;
            filter_r->d_reclen = namelist_r[i]->d_reclen;
            filter_r->d_type = namelist_r[i]->d_type;

            strcpy(filter_r->d_name, namelist_r[i]->d_name);
            filter_r->d_name[sizeof(namelist_r[i]->d_name) - 1] = '\0';

            if (filter != NULL && !(*filter)(filter_r)) {//apply filter
                free(filter_r);
                continue;
            }

            result[nItems++] = filter_r;
        }
        
        if (nItems && compar != NULL) // sort
            qsort(result, nItems, sizeof(struct bionic_dirent *), (__compar_fn_t)compar);

        *namelist = result;
    } else {
        return res;
    }

    return nItems;
}

ABI_ATTR int scandir_impl(const char *dir,
                      struct bionic_dirent ***namelist,
                      int (*filter) (const struct bionic_dirent *),
                      int (*compar) (const struct bionic_dirent **,
                                     const struct bionic_dirent **))
{
    return scandirat_impl(AT_FDCWD, dir, namelist, filter, compar);
}
