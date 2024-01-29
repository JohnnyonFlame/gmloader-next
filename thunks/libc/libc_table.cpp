// Behold the symbol-eating monster!
// This file contains all of the exports picked up by generate_libc.py and then
// some, and it's duty is to fix-up and export all of the necessary bionic libc
// symbols.
// Due to the many differences, we don't want to just load bionic, we
// want instead to provide a reasonable wrapper, this is said wrapper. 

#include "common.hpp"
#include "platform.h"
#include "so_util.h"
#include "thunk_gen.h"
#include "thunk_pthread.h"
#include "thunk_time64.h"
#include <signal.h>

//Some necessary types
typedef sigset_t sigset64_t;
typedef time64_t Time64_T;
typedef void (*dtor_func) (void *);

// Helpers to ensure we don't get issues with overloaded functions/returns due to C++
// taken from the BIONIC sources.
namespace C_FUNCS {
extern "C" {
#define restrict
#include "c_prototypes.h"
#undef restrict
};
};

// Function prototypes that we extracted using scripts/generate_libc.py
#include "impl_header.h"

// Some manually exported symbols
extern "C" int __cxa_atexit (void (*func) (void *), void *arg, void *d);
extern "C" void __cxa_finalize (void *d);
extern "C" int __cxa_thread_atexit_impl (dtor_func func, void *obj, void *dso_symbol);
extern void *_Unwind_RaiseException;
extern void *_Unwind_DeleteException;
extern void *_Unwind_SetGR;
extern void *_Unwind_SetIP;
extern void *_Unwind_GetLanguageSpecificData;
extern void *_Unwind_GetIP;
extern void *_Unwind_GetRegionStart;
extern void *_Unwind_Resume;
extern void *__stack_chk_fail;
extern void *__stack_chk_guard;
extern void __cxa_call_unexpected_impl(void*) {
    WARN_STUB
};
extern void *__cxa_begin_cleanup;
extern void *__cxa_type_match;
extern void *__gnu_Unwind_Find_exidx;


extern BIONIC_FILE __sF_fake[3];
extern BIONIC_FILE *stdin_impl;
extern BIONIC_FILE *stdout_impl;
extern BIONIC_FILE *stderr_impl;

extern const char *_ctype_impl;
extern const short *_tolower_tab_impl;
extern const short *_toupper_tab_impl;

DynLibFunction symtable_libc[] = {
    // Symbols picked up by generate_libc.py
    #include "impl_tab.h"

    // Some custom implementations, pointers to structs (__sF, _ctype_, ...), etc
    {"__sF", (uintptr_t)&__sF_fake},
    {"stdin", (uintptr_t)&stdin_impl},
    {"stdout", (uintptr_t)&stdout_impl},
    {"stderr", (uintptr_t)&stderr_impl},
    {"_stdin", (uintptr_t)&stdin_impl},
    {"_stdout", (uintptr_t)&stdout_impl},
    {"_stderr", (uintptr_t)&stderr_impl},
    {"__stack_chk_fail", (uintptr_t)&__stack_chk_fail},
    {"__stack_chk_guard", (uintptr_t)&__stack_chk_guard},
    {"__cxa_call_unexpected", (uintptr_t)&__cxa_call_unexpected_impl},
    {"__cxa_atexit", (uintptr_t)&__cxa_atexit},
    {"__cxa_finalize", (uintptr_t)&__cxa_finalize},
    {"__cxa_thread_atexit_impl", (uintptr_t)&__cxa_thread_atexit_impl},
    {"_ctype_", (uintptr_t)&_ctype_impl },
    {"_tolower_tab_", (uintptr_t)&_tolower_tab_impl },
    {"_toupper_tab_", (uintptr_t)&_toupper_tab_impl },
    {"_Unwind_RaiseException", (uintptr_t)&_Unwind_RaiseException},
    {"_Unwind_DeleteException", (uintptr_t)&_Unwind_DeleteException},
#if defined(__aarch64__)
    {"_Unwind_SetGR", (uintptr_t)&_Unwind_SetGR},
    {"_Unwind_SetIP", (uintptr_t)&_Unwind_SetIP},
    {"_Unwind_GetIP", (uintptr_t)&_Unwind_GetIP},
#endif
    {"_Unwind_GetLanguageSpecificData", (uintptr_t)&_Unwind_GetLanguageSpecificData},
    {"_Unwind_GetRegionStart", (uintptr_t)&_Unwind_GetRegionStart},
    {"_Unwind_Resume", (uintptr_t)&_Unwind_Resume},
#if defined(__arm__)
    {"__gnu_Unwind_Find_exidx", (uintptr_t)&__gnu_Unwind_Find_exidx},
#endif
    {NULL, (uintptr_t)NULL}
};