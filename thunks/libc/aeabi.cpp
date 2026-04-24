#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"

#if defined(__arm__) && !defined(__aarch64__)
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
 
ABI_ATTR double __adddf3_impl(double a, double b) { return a + b; }
ABI_ATTR float  __addsf3_impl(float  a, float  b) { return a + b; }

ABI_ATTR double __aeabi_dadd_impl (double a, double b) { return a + b; }
ABI_ATTR double __aeabi_dsub_impl (double a, double b) { return a - b; }
ABI_ATTR double __aeabi_drsub_impl(double a, double b) { return b - a; }
ABI_ATTR double __aeabi_dmul_impl (double a, double b) { return a * b; }
ABI_ATTR double __aeabi_ddiv_impl (double a, double b) { return a / b; }
ABI_ATTR float __aeabi_fadd_impl (float a, float b) { return a + b; }
ABI_ATTR float __aeabi_fsub_impl (float a, float b) { return a - b; }
ABI_ATTR float __aeabi_frsub_impl(float a, float b) { return b - a; }
ABI_ATTR float __aeabi_fmul_impl (float a, float b) { return a * b; }
ABI_ATTR float __aeabi_fdiv_impl (float a, float b) { return a / b; }
ABI_ATTR int __aeabi_dcmpeq_impl(double a, double b) { return (a == b) ? 1 : 0; }
ABI_ATTR int __aeabi_dcmplt_impl(double a, double b) { return (a <  b) ? 1 : 0; }
ABI_ATTR int __aeabi_dcmple_impl(double a, double b) { return (a <= b) ? 1 : 0; }
ABI_ATTR int __aeabi_dcmpge_impl(double a, double b) { return (a >= b) ? 1 : 0; }
ABI_ATTR int __aeabi_dcmpgt_impl(double a, double b) { return (a >  b) ? 1 : 0; }
 
ABI_ATTR int __aeabi_dcmpun_impl(double a, double b)
{
    return (a != a) | (b != b);
}
 
ABI_ATTR int __aeabi_fcmpeq_impl(float a, float b) { return (a == b) ? 1 : 0; }
ABI_ATTR int __aeabi_fcmplt_impl(float a, float b) { return (a <  b) ? 1 : 0; }
ABI_ATTR int __aeabi_fcmple_impl(float a, float b) { return (a <= b) ? 1 : 0; }
ABI_ATTR int __aeabi_fcmpge_impl(float a, float b) { return (a >= b) ? 1 : 0; }
ABI_ATTR int __aeabi_fcmpgt_impl(float a, float b) { return (a >  b) ? 1 : 0; }
 
ABI_ATTR int __aeabi_fcmpun_impl(float a, float b)
{
    return (a != a) | (b != b);
}
 
ABI_ATTR void __aeabi_cdcmpeq_impl(double a, double b)
{
#if defined(__ARM_FP) && (__ARM_FP & 0x8)          /* VFP-D16 or better */
    __asm__ volatile(
        "vcmp.f64 %P0, %P1\n\t"
        "vmrs     apsr_nzcv, fpscr"
        : : "w"(a), "w"(b) : "cc"
    );
#else
    uint32_t flags;
    int unordered = (a != a) | (b != b);
    if (unordered) {
        /* N=0 Z=0 C=0 V=1  – "unordered" result */
        flags = (1u << 28);                          /* V bit */
    } else if (a == b) {
        flags = (1u << 30) | (1u << 29);            /* Z=1, C=1 */
    } else if (a < b) {
        flags = (1u << 31);                          /* N=1 */
    } else {
        flags = (1u << 29);                          /* C=1 */
    }
    __asm__ volatile("msr apsr_nzcvq, %0" : : "r"(flags) : "cc");
#endif
}
 
/* cdcmple: a ≤ b  →  C set if a < b or unordered; Z set if a == b */
ABI_ATTR void __aeabi_cdcmple_impl(double a, double b)
{
#if defined(__ARM_FP) && (__ARM_FP & 0x8)
    __asm__ volatile(
        "vcmpe.f64 %P0, %P1\n\t"
        "vmrs      apsr_nzcv, fpscr"
        : : "w"(a), "w"(b) : "cc"
    );
#else
    uint32_t flags;
    int unordered = (a != a) | (b != b);
    if (unordered) {
        flags = (1u << 28);                          /* V=1 */
    } else if (a == b) {
        flags = (1u << 30) | (1u << 29);            /* Z=1, C=1 */
    } else if (a < b) {
        flags = (1u << 31);                          /* N=1 */
    } else {
        flags = (1u << 29);                          /* C=1 */
    }
    __asm__ volatile("msr apsr_nzcvq, %0" : : "r"(flags) : "cc");
#endif
}
 
/* cdrcmple: b ≤ a  (operands logically reversed from cdcmple) */
ABI_ATTR void __aeabi_cdrcmple_impl(double a, double b)
{
    /* Swap operands and delegate. */
    __aeabi_cdcmple_impl(b, a);
}
 
/* ---- single variants ---------------------------------------------------- */
 
ABI_ATTR void __aeabi_cfcmpeq_impl(float a, float b)
{
#if defined(__ARM_FP) && (__ARM_FP & 0x4)          /* VFP single-precision */
    __asm__ volatile(
        "vcmp.f32 %0, %1\n\t"
        "vmrs     apsr_nzcv, fpscr"
        : : "t"(a), "t"(b) : "cc"
    );
#else
    uint32_t flags;
    int unordered = (a != a) | (b != b);
    if (unordered) {
        flags = (1u << 28);
    } else if (a == b) {
        flags = (1u << 30) | (1u << 29);
    } else if (a < b) {
        flags = (1u << 31);
    } else {
        flags = (1u << 29);
    }
    __asm__ volatile("msr apsr_nzcvq, %0" : : "r"(flags) : "cc");
#endif
}
 
ABI_ATTR void __aeabi_cfcmple_impl(float a, float b)
{
#if defined(__ARM_FP) && (__ARM_FP & 0x4)
    __asm__ volatile(
        "vcmpe.f32 %0, %1\n\t"
        "vmrs      apsr_nzcv, fpscr"
        : : "t"(a), "t"(b) : "cc"
    );
#else
    uint32_t flags;
    int unordered = (a != a) | (b != b);
    if (unordered) {
        flags = (1u << 28);
    } else if (a == b) {
        flags = (1u << 30) | (1u << 29);
    } else if (a < b) {
        flags = (1u << 31);
    } else {
        flags = (1u << 29);
    }
    __asm__ volatile("msr apsr_nzcvq, %0" : : "r"(flags) : "cc");
#endif
}
 
ABI_ATTR void __aeabi_cfrcmple_impl(float a, float b)
{
    __aeabi_cfcmple_impl(b, a);
}
 
ABI_ATTR float    __aeabi_d2f_impl  (double a) { return  (float)   a; }
ABI_ATTR int32_t  __aeabi_d2iz_impl (double a) { return  (int32_t) a; }
ABI_ATTR uint32_t __aeabi_d2uiz_impl(double a) { return  (uint32_t)a; }
 
ABI_ATTR double   __aeabi_f2d_impl  (float a)  { return  (double)  a; }
ABI_ATTR int32_t  __aeabi_f2iz_impl (float a)  { return  (int32_t) a; }
ABI_ATTR uint32_t __aeabi_f2uiz_impl(float a)  { return  (uint32_t)a; }
 
ABI_ATTR double   __aeabi_i2d_impl  (int32_t a)  { return (double) a; }
ABI_ATTR float    __aeabi_i2f_impl  (int32_t a)  { return (float)  a; }
 
ABI_ATTR double   __aeabi_ui2d_impl (uint32_t a) { return (double) a; }
ABI_ATTR float    __aeabi_ui2f_impl (uint32_t a) { return (float)  a; }
 
ABI_ATTR double   __aeabi_l2d_impl  (int64_t a)  { return (double) a; }
ABI_ATTR float    __aeabi_l2f_impl  (int64_t a)  { return (float)  a; }
 
ABI_ATTR double   __aeabi_ul2d_impl (uint64_t a) { return (double) a; }
ABI_ATTR float    __aeabi_ul2f_impl (uint64_t a) { return (float)  a; }
  
ABI_ATTR int32_t __aeabi_idiv_impl(int32_t num, int32_t den)
{
    return num / den;
}
  
ABI_ATTR __aeabi_idivmod_result_t __aeabi_idivmod_impl(int32_t num, int32_t den)
{
    return (__aeabi_idivmod_result_t){ num / den, num % den };
}
  
ABI_ATTR uint32_t __aeabi_uidiv_impl(uint32_t num, uint32_t den)
{
    return num / den;
}
  
ABI_ATTR __aeabi_uidivmod_result_t __aeabi_uidivmod_impl(uint32_t num, uint32_t den)
{
    return (__aeabi_uidivmod_result_t){ num / den, num % den };
}
   
ABI_ATTR __aeabi_ldivmod_result_t __aeabi_ldivmod_impl(int64_t num, int64_t den)
{
    return (__aeabi_ldivmod_result_t){ num / den, num % den };
}
  
ABI_ATTR __aeabi_uldivmod_result_t __aeabi_uldivmod_impl(uint64_t num, uint64_t den)
{
    return (__aeabi_uldivmod_result_t){ num / den, num % den };
}
  
ABI_ATTR int32_t __aeabi_idiv0_impl(void) { return 0; }
ABI_ATTR int64_t __aeabi_ldiv0_impl(void) { return 0; }
  
ABI_ATTR uint64_t __aeabi_llsl_impl(uint64_t val, int shift)
{
    if ((unsigned)shift >= 64u) return 0u;
    return val << shift;
}
 
ABI_ATTR uint64_t __aeabi_llsr_impl(uint64_t val, int shift)
{
    if ((unsigned)shift >= 64u) return 0u;
    return val >> shift;
}
 
ABI_ATTR int64_t __aeabi_lasr_impl(int64_t val, int shift)
{
    if ((unsigned)shift >= 64u) return val >> 63;   /* all-sign-bits */
    return val >> shift;
}
  
ABI_ATTR int64_t __aeabi_lmul_impl(int64_t a, int64_t b)
{
    return a * b;
}

ABI_ATTR void *__aeabi_read_tp_impl(void)
{
    void *tp;
#if defined(__ARM_ARCH) && __ARM_ARCH >= 6
    __asm__ volatile(
        "mrc p15, 0, %0, c13, c0, 3"   /* TPIDRURO */
        : "=r"(tp)
    );
#else
    /* Fallback for pre-v6 cores or non-ARM hosts: return NULL.
     * A real port must supply this via a platform-specific mechanism. */
    tp = (void *)0;
#endif
    return tp;
}
#endif