#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-macro-redefined"

#ifdef IS_TAB_GEN
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <bits/types/locale_t.h>
#include <bits/types/wint_t.h>
#include <bits/types/FILE.h>
#include <bits/types/mbstate_t.h>
#undef __attribute_pure__
#undef __attribute_const__
#endif

#define __INTRODUCED_IN(...)
#define __RENAME(...)
#define __attribute_pure__
#define __attribute_const__
#define __RENAME_LDBL(...)
#define __RENAME_LDBL_NO_GUARD_FOR_NDK(...)
#define __INTRODUCED_IN_X86_NO_GUARD_FOR_NDK(...)
#define __INTRODUCED_IN_NO_GUARD_FOR_NDK(...)
#define __wur 
#define __BIONIC_ALLOC_SIZE(...)
#define __mallocfunc
#define __printflike(...)
#define __scanflike(...)
#pragma GCC diagnostic pop

void* memccpy(void* __dst, const void* __src, int __stop_char, size_t __n);
void* memchr(const void* __s, int __ch, size_t __n) __attribute_pure__;
void* memrchr(const void* __s, int __ch, size_t __n) __attribute_pure__;
int memcmp(const void* __lhs, const void* __rhs, size_t __n) __attribute_pure__;
void* memcpy(void*, const void*, size_t);
void* mempcpy(void* __dst, const void* __src, size_t __n) __INTRODUCED_IN(23);
void* memmove(void* __dst, const void* __src, size_t __n);
void* memset(void* __dst, int __ch, size_t __n);
void* memmem(const void* __haystack, size_t __haystack_size, const void* __needle, size_t __needle_size) __attribute_pure__;
char* strchr(const char* __s, int __ch) __attribute_pure__;
char* __strchr_chk(const char* __s, int __ch, size_t __n) __INTRODUCED_IN(18);
char* strchrnul(const char* __s, int __ch) __attribute_pure__ __INTRODUCED_IN(24);
char* strrchr(const char* __s, int __ch) __attribute_pure__;
char* __strrchr_chk(const char* __s, int __ch, size_t __n) __INTRODUCED_IN(18);
size_t strlen(const char* __s) __attribute_pure__;
size_t __strlen_chk(const char* __s, size_t __n) __INTRODUCED_IN(17);
int strcmp(const char* __lhs, const char* __rhs) __attribute_pure__;
char* stpcpy(char* __dst, const char* __src) __INTRODUCED_IN(21);
char* strcpy(char* __dst, const char* __src);
char* strcat(char* __dst, const char* __src);
char* strdup(const char* __s);
char* strstr(const char* __haystack, const char* __needle) __attribute_pure__;
char* strcasestr(const char* __haystack, const char* __needle) __attribute_pure__;
char* strtok(char* __s, const char* __delimiter);
char* strtok_r(char* __s, const char* __delimiter, char** __pos_ptr);
char* strerror(int __errno_value);
char* strerror_l(int __errno_value, locale_t __l) __INTRODUCED_IN(23);
int strerror_r(int __errno_value, char* __buf, size_t __n);
size_t strnlen(const char* __s, size_t __n) __attribute_pure__;
char* strncat(char* __dst, const char* __src, size_t __n);
char* strndup(const char* __s, size_t __n);
int strncmp(const char* __lhs, const char* __rhs, size_t __n) __attribute_pure__;
char* stpncpy(char* __dst, const char* __src, size_t __n) __INTRODUCED_IN(21);
char* strncpy(char* __dst, const char* __src, size_t __n);

size_t strlcat(char* __dst, const char* __src, size_t __n);
size_t strlcpy(char* __dst, const char* __src, size_t __n);

size_t strcspn(const char* __s, const char* __reject) __attribute_pure__;
char* strpbrk(const char* __s, const char* __accept) __attribute_pure__;
char* strsep(char** __s_ptr, const char* __delimiter);
size_t strspn(const char* __s, const char* __accept);

char* strsignal(int __signal);

int strcoll(const char* __lhs, const char* __rhs) __attribute_pure__;
size_t strxfrm(char* __dst, const char* __src, size_t __n);

int strcoll_l(const char* __lhs, const char* __rhs, locale_t __l) __attribute_pure__ __INTRODUCED_IN(21);
size_t strxfrm_l(char* __dst, const char* __src, size_t __n, locale_t __l) __INTRODUCED_IN(21);

char* basename(const char* __path) __RENAME(__gnu_basename) __INTRODUCED_IN(23);

int abs(int __x) __attribute_const__ __INTRODUCED_IN(19);
long labs(long __x) __attribute_const__ __INTRODUCED_IN(19);
long long llabs(long long __x) __attribute_const__ __INTRODUCED_IN(19);

float strtof(const char* __s, char** __end_ptr) __INTRODUCED_IN(21);
double atof(const char* __s) __attribute_pure__ __INTRODUCED_IN(21);
int rand(void) __INTRODUCED_IN(21);
void srand(unsigned int __seed) __INTRODUCED_IN(21);
long random(void) __INTRODUCED_IN(21);
void srandom(unsigned int __seed) __INTRODUCED_IN(21);
int grantpt(int __fd) __INTRODUCED_IN(21);

long long strtoll_l(const char* __s, char** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(21);
unsigned long long strtoull_l(const char* __s, char** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(21);
long double strtold_l(const char* __s, char** __end_ptr, locale_t __l) __INTRODUCED_IN(21);

double strtod_l(const char* __s, char** __end_ptr, locale_t __l) __INTRODUCED_IN(26);
float strtof_l(const char* __s, char** __end_ptr, locale_t __l) __INTRODUCED_IN(26);
long strtol_l(const char* __s, char** __end_ptr, int, locale_t __l) __INTRODUCED_IN(26);

double acos(double __x);
float acosf(float __x);
long double acosl(long double __x) __RENAME_LDBL(acos, 3, 21);

double asin(double __x);
float asinf(float __x);
long double asinl(long double __x) __RENAME_LDBL(asin, 3, 21);

double atan(double __x);
float atanf(float __x);
long double atanl(long double __x) __RENAME_LDBL(atan, 3, 21);

double atan2(double __y, double __x);
float atan2f(float __y, float __x);
long double atan2l(long double __y, long double __x) __RENAME_LDBL(atan2, 3, 21);

double cos(double __x);
float cosf(float __x);
long double cosl(long double __x) __RENAME_LDBL(cos, 3, 21);

double sin(double __x);
float sinf(float __x);
long double sinl(long double __x) __RENAME_LDBL(sin, 3, 21);

double tan(double __x);
float tanf(float __x);
long double tanl(long double __x) __RENAME_LDBL(tan, 3, 21);

double acosh(double __x);
float acoshf(float __x);
long double acoshl(long double __x) __RENAME_LDBL(acosh, 3, 21);

double asinh(double __x);
float asinhf(float __x);
long double asinhl(long double __x) __RENAME_LDBL(asinh, 3, 21);

double atanh(double __x);
float atanhf(float __x);
long double atanhl(long double __x) __RENAME_LDBL(atanh, 3, 21);

double cosh(double __x);
float coshf(float __x);
long double coshl(long double __x) __RENAME_LDBL(cosh, 3, 21);

double sinh(double __x);
float sinhf(float __x);
long double sinhl(long double __x) __RENAME_LDBL(sinh, 3, 21);

double tanh(double __x);
float tanhf(float __x);
long double tanhl(long double __x) __RENAME_LDBL(tanh, 3, 21);

double exp(double __x);
float expf(float __x);
long double expl(long double __x) __RENAME_LDBL(exp, 3, 21);

double exp2(double __x);
float exp2f(float __x);
long double exp2l(long double __x) __RENAME_LDBL(exp2, 3, 21);

double expm1(double __x);
float expm1f(float __x);
long double expm1l(long double __x) __RENAME_LDBL(expm1, 3, 21);

double frexp(double __x, int* __exponent);
float frexpf(float __x, int* __exponent);
long double frexpl(long double __x, int* __exponent) __RENAME_LDBL(frexp, 3, 21);

int ilogb(double __x) __attribute_const__;
int ilogbf(float __x) __attribute_const__;
int ilogbl(long double __x) __RENAME_LDBL(ilogb, 3, 3) __attribute_const__;

double ldexp(double __x, int __exponent);
float ldexpf(float __x, int __exponent);
long double ldexpl(long double __x, int __exponent) __RENAME_LDBL(ldexp, 3, 3);

double log(double __x);
float logf(float __x);
long double logl(long double __x) __RENAME_LDBL(log, 3, 21);

double log10(double __x);
float log10f(float __x);
long double log10l(long double __x) __RENAME_LDBL(log10, 3, 21);

double log1p(double __x);
float log1pf(float __x);
long double log1pl(long double __x) __RENAME_LDBL(log1p, 3, 21);

double log2(double __x) __INTRODUCED_IN(18);
float log2f(float __x) __INTRODUCED_IN(18);
long double log2l(long double __x) __RENAME_LDBL(log2, 18, 18);

double logb(double __x);
float logbf(float __x);
long double logbl(long double __x) __RENAME_LDBL(logb, 3, 18);

double modf(double __x, double* __integral_part);
float modff(float __x, float* __integral_part);
long double modfl(long double __x, long double* __integral_part) __RENAME_LDBL(modf, 3, 21);

double scalbn(double __x, int __exponent);
float scalbnf(float __x, int __exponent);
long double scalbnl(long double __x, int __exponent) __RENAME_LDBL(scalbn, 3, 3);

/* TODO: once the NDK only supports >= 18, use __RENAME_LDBL here too. */
double scalbln(double __x, long __exponent) __INTRODUCED_IN_X86_NO_GUARD_FOR_NDK(18);
float scalblnf(float __x, long __exponent) __INTRODUCED_IN_X86_NO_GUARD_FOR_NDK(18);
long double scalblnl(long double __x, long __exponent) __INTRODUCED_IN_X86_NO_GUARD_FOR_NDK(18);

double cbrt(double __x);
float cbrtf(float __x);
long double cbrtl(long double __x) __RENAME_LDBL(cbrt, 3, 21);

double fabs(double __x) __attribute_const__;
float fabsf(float __x) __attribute_const__;
long double fabsl(long double __x) __RENAME_LDBL(fabs, 3, 3) __attribute_const__;

double hypot(double __x, double __y);
float hypotf(float __x, float __y);
long double hypotl(long double __x, long double __y) __RENAME_LDBL(hypot, 3, 21);

double pow(double __x, double __y);
float powf(float __x, float __y);
long double powl(long double __x, long double __y) __RENAME_LDBL(pow, 3, 21);

double sqrt(double __x);
float sqrtf(float __x);
long double sqrtl(long double __x) __RENAME_LDBL(sqrt, 3, 21);

double erf(double __x);
float erff(float __x);
long double erfl(long double __x) __RENAME_LDBL(erf, 3, 21);

double erfc(double __x);
float erfcf(float __x);
long double erfcl(long double __x) __RENAME_LDBL(erfc, 3, 21);

double lgamma(double __x);
float lgammaf(float __x);
long double lgammal(long double __x) __RENAME_LDBL(lgamma, 3, 21);

double tgamma(double __x);
float tgammaf(float __x);
long double tgammal(long double __x) __RENAME_LDBL(tgamma, 3, 21);

double ceil(double __x);
float ceilf(float __x);
long double ceill(long double __x) __RENAME_LDBL(ceil, 3, 3);

double floor(double __x);
float floorf(float __x);
long double floorl(long double __x) __RENAME_LDBL(floor, 3, 3);

double nearbyint(double __x);
float nearbyintf(float __x);
long double nearbyintl(long double __x) __RENAME_LDBL(nearbyint, 3, 21);

double rint(double __x);
float rintf(float __x);
long double rintl(long double __x) __RENAME_LDBL(rint, 3, 21);

long lrint(double __x);
long lrintf(float __x);
long lrintl(long double __x) __RENAME_LDBL(lrint, 3, 21);

long long llrint(double __x);
long long llrintf(float __x);
long long llrintl(long double __x) __RENAME_LDBL(llrint, 3, 21);

double round(double __x);
float roundf(float __x);
long double roundl(long double __x) __RENAME_LDBL(roundl, 3, 3);

long lround(double __x);
long lroundf(float __x);
long lroundl(long double __x) __RENAME_LDBL(lround, 3, 3);

long long llround(double __x);
long long llroundf(float __x);
long long llroundl(long double __x) __RENAME_LDBL(llround, 3, 3);

double trunc(double __x);
float truncf(float __x);
long double truncl(long double __x) __RENAME_LDBL(trunc, 3, 3);

double fmod(double __x, double __y);
float fmodf(float __x, float __y);
long double fmodl(long double __x, long double __y) __RENAME_LDBL(fmod, 3, 21);

double remainder(double __x, double __y);
float remainderf(float __x, float __y);
long double remainderl(long double __x, long double __y) __RENAME_LDBL(remainder, 3, 21);

double remquo(double __x, double __y, int* __quotient_bits);
float remquof(float __x, float __y, int* __quotient_bits);
long double remquol(long double __x, long double __y, int* __quotient_bits) __RENAME_LDBL(remquo, 3, 21);

double copysign(double __value, double __sign) __attribute_const__;
float copysignf(float __value, float __sign) __attribute_const__;
long double copysignl(long double __value, long double __sign) __RENAME_LDBL(copysign, 3, 3) __attribute_const__;

double nan(const char* __kind) __attribute_const__;
float nanf(const char* __kind) __attribute_const__;
long double nanl(const char* __kind) __RENAME_LDBL(nan, 13, 13) __attribute_const__;

double nextafter(double __x, double __y);
float nextafterf(float __x, float __y);
long double nextafterl(long double __x, long double __y) __RENAME_LDBL_NO_GUARD_FOR_NDK(nextafter, 3, 21);

double nexttoward(double __x, long double __y) __INTRODUCED_IN_NO_GUARD_FOR_NDK(18);
float nexttowardf(float __x, long double __y);
long double nexttowardl(long double __x, long double __y) __RENAME_LDBL_NO_GUARD_FOR_NDK(nexttoward, 18, 18);

double fdim(double __x, double __y);
float fdimf(float __x, float __y);
long double fdiml(long double __x, long double __y) __RENAME_LDBL(fdim, 3, 3);

double fmax(double __x, double __y) __attribute_const__;
float fmaxf(float __x, float __y) __attribute_const__;
long double fmaxl(long double __x, long double __y) __RENAME_LDBL(fmax, 3, 3) __attribute_const__;

double fmin(double __x, double __y) __attribute_const__;
float fminf(float __x, float __y) __attribute_const__;
long double fminl(long double __x, long double __y) __RENAME_LDBL(fmin, 3, 3) __attribute_const__;

double fma(double __x, double __y, double __z);
float fmaf(float __x, float __y, float __z);
long double fmal(long double __x, long double __y, long double __z) __RENAME_LDBL_NO_GUARD_FOR_NDK(fma, 3, 21);

int (isinf)(double __x) __attribute_const__ __INTRODUCED_IN(21);
int (isnan)(double __x) __attribute_const__;

/* POSIX extensions. */
extern int signgam;

double j0(double __x);
double j1(double __x);
double jn(int __n, double __x);
double y0(double __x);
double y1(double __x);
double yn(int __n, double __x);

double gamma(double __x);
double scalb(double __x, double __exponent);
double drem(double __x, double __y);
int finite(double __x) __attribute_const__;
int isnanf(float __x) __attribute_const__;
double gamma_r(double __x, int* __sign);
double lgamma_r(double __x, int* __sign);
double significand(double __x);
long double lgammal_r(long double __x, int* __sign) __INTRODUCED_IN(23);
long double significandl(long double __x) __INTRODUCED_IN(21);
float dremf(float __x, float __y);
int finitef(float __x) __attribute_const__;
float gammaf(float __x);
float j0f(float __x);
float j1f(float __x);
float jnf(int __n, float __x);
float scalbf(float __x, float __exponent);
float y0f(float __x);
float y1f(float __x);
float ynf(int __n, float __x);
float gammaf_r(float __x, int* __sign);
float lgammaf_r(float __x, int* __sign);
float significandf(float __x);
void sincos(double __x, double* __sin, double* __cos);
void sincosf(float __x, float* __sin, float* __cos);
void sincosl(long double __x, long double* __sin, long double* __cos);

void* malloc(size_t __byte_count) __mallocfunc __BIONIC_ALLOC_SIZE(1) __wur;
void* calloc(size_t __item_count, size_t __item_size) __mallocfunc __BIONIC_ALLOC_SIZE(1,2) __wur;
void* realloc(void* __ptr, size_t __byte_count) __BIONIC_ALLOC_SIZE(2) __wur;
void* reallocarray(void* __ptr, size_t __item_count, size_t __item_size) __BIONIC_ALLOC_SIZE(2, 3) __wur __INTRODUCED_IN(29);
void free(void* __ptr);
void* memalign(size_t __alignment, size_t __byte_count) __mallocfunc __BIONIC_ALLOC_SIZE(2) __wur;
size_t malloc_usable_size(const void* __ptr) __INTRODUCED_IN(17);
struct mallinfo mallinfo(void);
int malloc_info(int __must_be_zero, FILE* __fp) __INTRODUCED_IN(23);
int mallopt(int __option, int __value) __INTRODUCED_IN(26);

int snprintf(char* __buf, size_t __size, const char* __fmt, ...) __printflike(3, 4);
int vfscanf(FILE* __fp, const char* __fmt, va_list __args) __scanflike(2, 0);
int vscanf(const char* __fmt , va_list __args) __scanflike(1, 0);
int vsnprintf(char* __buf, size_t __size, const char* __fmt, va_list __args) __printflike(3, 0);
int vsscanf(const char* __s, const char* __fmt, va_list __args) __scanflike(2, 0);

wint_t btowc(int __ch);
int fwprintf(FILE* __fp, const wchar_t* __fmt, ...);
int fwscanf(FILE* __fp, const wchar_t* __fmt, ...);
wint_t fgetwc(FILE* __fp);
wchar_t* fgetws(wchar_t* __buf, int __size, FILE* __fp);
wint_t fputwc(wchar_t __wc, FILE* __fp);
int fputws(const wchar_t* __s, FILE* __fp);
int fwide(FILE* __fp, int __mode);
wint_t getwc(FILE* __fp);
wint_t getwchar(void);
int mbsinit(const mbstate_t* __ps);
size_t mbrlen(const char* __s, size_t __n, mbstate_t* __ps);
size_t mbrtowc(wchar_t* __buf, const char* __s, size_t __n, mbstate_t* __ps);
size_t mbsrtowcs(wchar_t* __dst, const char** __src, size_t __dst_n, mbstate_t* __ps);
size_t mbsnrtowcs(wchar_t* __dst, const char** __src, size_t __src_n, size_t __dst_n, mbstate_t* __ps) __INTRODUCED_IN(21);
wint_t putwc(wchar_t __wc, FILE* __fp);
wint_t putwchar(wchar_t __wc);
int swprintf(wchar_t* __buf, size_t __n, const wchar_t* __fmt, ...);
int swscanf(const wchar_t* __s, const wchar_t* __fmt, ...);
wint_t ungetwc(wint_t __wc, FILE* __fp);
int vfwprintf(FILE* __fp, const wchar_t* __fmt, va_list __args);
int vfwscanf(FILE* __fp, const wchar_t* __fmt, va_list __args) __INTRODUCED_IN(21);
int vswprintf(wchar_t* __buf, size_t __n, const wchar_t* __fmt, va_list __args);
int vswscanf(const wchar_t* __s, const wchar_t* __fmt, va_list __args) __INTRODUCED_IN(21);
int vwprintf(const wchar_t* __fmt, va_list __args);
int vwscanf(const wchar_t* __fmt, va_list __args) __INTRODUCED_IN(21);
wchar_t* wcpcpy(wchar_t* __dst, const wchar_t* __src);
wchar_t* wcpncpy(wchar_t* __dst, const wchar_t* __src, size_t __n);
size_t wcrtomb(char* __buf, wchar_t __wc, mbstate_t* __ps);
int wcscasecmp(const wchar_t* __lhs, const wchar_t* __rhs);
int wcscasecmp_l(const wchar_t* __lhs, const wchar_t* __rhs, locale_t __l) __INTRODUCED_IN(23);
wchar_t* wcscat(wchar_t* __dst, const wchar_t* __src);
wchar_t* wcschr(const wchar_t* __s, wchar_t __wc);
int wcscmp(const wchar_t* __lhs, const wchar_t* __rhs);
int wcscoll(const wchar_t* __lhs, const wchar_t* __rhs);
wchar_t* wcscpy(wchar_t* __dst, const wchar_t* __src);
size_t wcscspn(const wchar_t* __s, const wchar_t* __accept);
size_t wcsftime(wchar_t* __buf, size_t __n, const wchar_t* __fmt, const struct tm* __tm);
size_t wcsftime_l(wchar_t* __buf, size_t __n, const wchar_t* __fmt, const struct tm* __tm, locale_t __l) __INTRODUCED_IN(28);
size_t wcslen(const wchar_t* __s);
int wcsncasecmp(const wchar_t* __lhs, const wchar_t* __rhs, size_t __n);
int wcsncasecmp_l(const wchar_t* __lhs, const wchar_t* __rhs, size_t __n, locale_t __l) __INTRODUCED_IN(23);
wchar_t* wcsncat(wchar_t* __dst, const wchar_t* __src, size_t __n);
int wcsncmp(const wchar_t* __lhs, const wchar_t* __rhs, size_t __n);
wchar_t* wcsncpy(wchar_t* __dst, const wchar_t* __src, size_t __n);
size_t wcsnrtombs(char* __dst, const wchar_t** __src, size_t __src_n, size_t __dst_n, mbstate_t* __ps) __INTRODUCED_IN(21);
wchar_t* wcspbrk(const wchar_t* __s, const wchar_t* __accept);
wchar_t* wcsrchr(const wchar_t* __s, wchar_t __wc);
size_t wcsrtombs(char* __dst, const wchar_t** __src, size_t __dst_n, mbstate_t* __ps);
size_t wcsspn(const wchar_t* __s, const wchar_t* __accept);
wchar_t* wcsstr(const wchar_t* __haystack, const wchar_t* __needle);
double wcstod(const wchar_t* __s, wchar_t** __end_ptr);
double wcstod_l(const wchar_t* __s, wchar_t** __end_ptr, locale_t __l) __INTRODUCED_IN(28);
float wcstof(const wchar_t* __s, wchar_t** __end_ptr) __INTRODUCED_IN(21);
float wcstof_l(const wchar_t* __s, wchar_t** __end_ptr, locale_t __l) __INTRODUCED_IN(28);
wchar_t* wcstok(wchar_t* __s, const wchar_t* __delimiter, wchar_t** __ptr);
long wcstol(const wchar_t* __s, wchar_t** __end_ptr, int __base);
long wcstol_l(const wchar_t* __s, wchar_t** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(28);
long long wcstoll(const wchar_t* __s, wchar_t** __end_ptr, int __base) __INTRODUCED_IN(21);
long double wcstold(const wchar_t* __s, wchar_t** __end_ptr) __RENAME_LDBL(wcstod, 3, 21);
unsigned long wcstoul(const wchar_t* __s, wchar_t** __end_ptr, int __base);
unsigned long wcstoul_l(const wchar_t* __s, wchar_t** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(28);
unsigned long long wcstoull(const wchar_t* __s, wchar_t** __end_ptr, int __base) __INTRODUCED_IN(21);
int wcswidth(const wchar_t* __s, size_t __n);
size_t wcsxfrm(wchar_t* __dst, const wchar_t* __src, size_t __n);
int wctob(wint_t __wc);
int wcwidth(wchar_t __wc);
wchar_t* wmemchr(const wchar_t* __src, wchar_t __wc, size_t __n);
int wmemcmp(const wchar_t* __lhs, const wchar_t* __rhs, size_t __n);
wchar_t* wmemcpy(wchar_t* __dst, const wchar_t* __src, size_t __n);
wchar_t* wmempcpy(wchar_t* __dst, const wchar_t* __src, size_t __n) __INTRODUCED_IN(23);
wchar_t* wmemmove(wchar_t* __dst, const wchar_t* __src, size_t __n);
wchar_t* wmemset(wchar_t* __dst, wchar_t __wc, size_t __n);
int wprintf(const wchar_t* __fmt, ...);
int wscanf(const wchar_t* __fmt, ...);

long long wcstoll_l(const wchar_t* __s, wchar_t** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(21);
unsigned long long wcstoull_l(const wchar_t* __s, wchar_t** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(21);
long double wcstold_l(const wchar_t* __s, wchar_t** __end_ptr, locale_t __l) __INTRODUCED_IN(21);

int wcscoll_l(const wchar_t* __lhs, const wchar_t* __rhs, locale_t __l) __attribute_pure__
    __INTRODUCED_IN(21);
size_t wcsxfrm_l(wchar_t* __dst, const wchar_t* __src, size_t __n, locale_t __l) __INTRODUCED_IN(21);

size_t wcslcat(wchar_t* __dst, const wchar_t* __src, size_t __n);
size_t wcslcpy(wchar_t* __dst, const wchar_t* __src, size_t __n);

FILE* open_wmemstream(wchar_t** __ptr, size_t* __size_ptr) __INTRODUCED_IN(23);
wchar_t* wcsdup(const wchar_t* __s);
size_t wcsnlen(const wchar_t* __s, size_t __n);

char* getenv(const char* __name);
int putenv(char* __assignment);
int setenv(const char* __name, const char* __value, int __overwrite);
int unsetenv(const char* __name);
int clearenv(void);

char* mkdtemp(char* __template);
char* mktemp(char* __template) __attribute__((deprecated("mktemp is unsafe, use mkstemp or tmpfile instead")));

int mkostemp64(char* __template, int __flags) __INTRODUCED_IN(23);
int mkostemp(char* __template, int __flags) __INTRODUCED_IN(23);
int mkostemps64(char* __template, int __suffix_length, int __flags) __INTRODUCED_IN(23);
int mkostemps(char* __template, int __suffix_length, int __flags) __INTRODUCED_IN(23);
int mkstemp64(char* __template) __INTRODUCED_IN(21);
int mkstemp(char* __template);
int mkstemps64(char* __template, int __flags) __INTRODUCED_IN(23);
int mkstemps(char* __template, int __flags);

long strtol(const char* __s, char** __end_ptr, int __base);
long long strtoll(const char* __s, char** __end_ptr, int __base);
unsigned long strtoul(const char* __s, char** __end_ptr, int __base);
unsigned long long strtoull(const char* __s, char** __end_ptr, int __base);

int posix_memalign(void** __memptr, size_t __alignment, size_t __size) __INTRODUCED_IN(17);

void* aligned_alloc(size_t __alignment, size_t __size) __INTRODUCED_IN(28);

double strtod(const char* __s, char** __end_ptr);
long double strtold(const char* __s, char** __end_ptr) __RENAME_LDBL(strtod, 3, 21);

unsigned long strtoul_l(const char* __s, char** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(26);

int atoi(const char* __s) __attribute_pure__;
long atol(const char* __s) __attribute_pure__;
long long atoll(const char* __s) __attribute_pure__;

__wur char* realpath(const char* __path, char* __resolved);
int system(const char* __command);

void* bsearch(const void* __key, const void* __base, size_t __nmemb, size_t __size, int (*__comparator)(const void* __lhs, const void* __rhs));

void qsort(void* __base, size_t __nmemb, size_t __size, int (*__comparator)(const void* __lhs, const void* __rhs));

uint32_t arc4random(void);
uint32_t arc4random_uniform(uint32_t __upper_bound);
void arc4random_buf(void* __buf, size_t __n);

int rand_r(unsigned int* __seed_ptr) __INTRODUCED_IN(21);

double drand48(void);
double erand48(unsigned short __xsubi[3]);
long jrand48(unsigned short __xsubi[3]);
void lcong48(unsigned short __param[7]) __INTRODUCED_IN(23);
long lrand48(void);
long mrand48(void);
long nrand48(unsigned short __xsubi[3]);
unsigned short* seed48(unsigned short __seed16v[3]);
void srand48(long __seed);

char* initstate(unsigned int __seed, char* __state, size_t __n) __INTRODUCED_IN(21);
char* setstate(char* __state) __INTRODUCED_IN(21);

int getpt(void);
int posix_openpt(int __flags) __INTRODUCED_IN(21);
char* ptsname(int __fd);
int ptsname_r(int __fd, char* __buf, size_t __n);
int unlockpt(int __fd);

int getsubopt(char** __option, char* const* __tokens, char** __value_ptr) __INTRODUCED_IN(26);

typedef struct {
  int quot;
  int rem;
} div_t;

div_t div(int __numerator, int __denominator) __attribute_const__;

typedef struct {
  long int quot;
  long int rem;
} ldiv_t;

ldiv_t ldiv(long __numerator, long __denominator) __attribute_const__;

typedef struct {
  long long int quot;
  long long int rem;
} lldiv_t;

lldiv_t lldiv(long long __numerator, long long __denominator) __attribute_const__;
int getloadavg(double __averages[], int __n) __INTRODUCED_IN(29);
const char* getprogname(void) __INTRODUCED_IN(21);
void setprogname(const char* __name) __INTRODUCED_IN(21);
int mblen(const char* __s, size_t __n) __INTRODUCED_IN_NO_GUARD_FOR_NDK(26);
size_t mbstowcs(wchar_t* __dst, const char* __src, size_t __n);
int mbtowc(wchar_t* __wc_ptr, const char* __s, size_t __n) __INTRODUCED_IN_NO_GUARD_FOR_NDK(21);
int wctomb(char* __dst, wchar_t __wc) __INTRODUCED_IN_NO_GUARD_FOR_NDK(21);
size_t wcstombs(char* __dst, const wchar_t* __src, size_t __n);
size_t __ctype_get_mb_cur_max(void) __INTRODUCED_IN(21);
int abs(int __x) __attribute_const__ __INTRODUCED_IN(19);
long labs(long __x) __attribute_const__ __INTRODUCED_IN(19);
long long llabs(long long __x) __attribute_const__ __INTRODUCED_IN(19);
float strtof(const char* __s, char** __end_ptr) __INTRODUCED_IN(21);
double atof(const char* __s) __attribute_pure__ __INTRODUCED_IN(21);
int rand(void) __INTRODUCED_IN(21);
void srand(unsigned int __seed) __INTRODUCED_IN(21);
long random(void) __INTRODUCED_IN(21);
void srandom(unsigned int __seed) __INTRODUCED_IN(21);
int grantpt(int __fd) __INTRODUCED_IN(21);
long long strtoll_l(const char* __s, char** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(21);
unsigned long long strtoull_l(const char* __s, char** __end_ptr, int __base, locale_t __l) __INTRODUCED_IN(21);
long double strtold_l(const char* __s, char** __end_ptr, locale_t __l) __INTRODUCED_IN(21);
double strtod_l(const char* __s, char** __end_ptr, locale_t __l) __INTRODUCED_IN(26);
float strtof_l(const char* __s, char** __end_ptr, locale_t __l) __INTRODUCED_IN(26);
long strtol_l(const char* __s, char** __end_ptr, int, locale_t __l) __INTRODUCED_IN(26);
char *index(const char *p, int ch);
wchar_t *wcswcs (const wchar_t *__haystack, const wchar_t *__needle);