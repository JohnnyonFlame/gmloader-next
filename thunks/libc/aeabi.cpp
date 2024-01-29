#include <stdlib.h>
#include <string.h>

#include "platform.h"

ABI_ATTR int __aeabi_atexit_impl(void *object, void (*destructor) (void *), void *dso_handle) {
    /* crap */
    return 0;
}

ABI_ATTR void __aeabi_memcpy8_impl(void *dest, const void *src, size_t n) {
    memcpy(dest, src, n);
}

ABI_ATTR void __aeabi_memcpy4_impl(void *dest, const void *src, size_t n) {
    memcpy(dest, src, n);
}

ABI_ATTR void __aeabi_memcpy_impl(void *dest, const void *src, size_t n) {
    memcpy(dest, src, n);
}

ABI_ATTR void __aeabi_memmove8_impl(void *dest, const void *src, size_t n) {
    memmove(dest, src, n);
}

ABI_ATTR void __aeabi_memmove4_impl(void *dest, const void *src, size_t n) {
    memmove(dest, src, n);
}

ABI_ATTR void __aeabi_memmove_impl(void *dest, const void *src, size_t n) {
    memmove(dest, src, n);
}

ABI_ATTR void __aeabi_memset8_impl(void *dest, size_t n, int c) {
    memset(dest, c, n);
}

ABI_ATTR void __aeabi_memset4_impl(void *dest, size_t n, int c) {
    memset(dest, c, n);
}

ABI_ATTR void __aeabi_memset_impl(void *dest, size_t n, int c) {
    memset(dest, c, n);
}

ABI_ATTR void __aeabi_memclr8_impl(void *dest, size_t n) {
    __aeabi_memset8_impl(dest, n, 0);
}

ABI_ATTR void __aeabi_memclr4_impl(void *dest, size_t n) {
    __aeabi_memset4_impl(dest, n, 0);
}

ABI_ATTR void __aeabi_memclr_impl(void *dest, size_t n) {
    __aeabi_memset_impl(dest, n, 0);
}
