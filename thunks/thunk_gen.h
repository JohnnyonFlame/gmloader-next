// Compile time code-generation for conditional softfp->hardfp thunks using template specialization.
// As of June 2022 clang still doesn't generate correct code for this mixed ABI situation, use gcc.

// Special Thanks for the folks at FEX-Emu's discord for inspiration/help with this.
// Free software, Licensed under BSD-0.

#pragma once
#include <type_traits>
#include <typeinfo>
#include <functional>
#include <string>

#include "platform.h"
#include "libc/bionic_file.h"

// Can't compare functions, but you can absolutely compare two different types
// which have been specialized with the same function (as long as it's an extern
// storage one), so we need this wrapper 
template <auto F>
struct isf_thin_wrapper {};

template <auto F1, auto F2>
struct is_same_function : std::is_same<isf_thin_wrapper<F1>, isf_thin_wrapper<F2>> { };

// Check if a function has floating point arguments
template <typename... Args>
struct has_float_arg: std::integral_constant<bool, false> {};

template <typename T, typename... Args>
struct has_float_arg<T, Args...>: std::integral_constant<bool, std::is_floating_point_v<T> || has_float_arg<Args...>::value> { };

// Check if a function has FILE* arguments
template <typename... Args>
struct has_FILEp_arg: std::integral_constant<bool, false> {};

template <typename T, typename... Args>
struct has_FILEp_arg<T, Args...>: std::integral_constant<bool, std::is_same_v<T, FILE*> || has_FILEp_arg<Args...>::value> { };

#ifdef WANTS_TRACE
#include <mutex>

// C++20: Print individual value by type
template<typename T>
inline void print_value(const T& value) {
    if constexpr (std::is_function_v<std::remove_pointer_t<T>>) {
        printf("%p", reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(value)));
    } else if constexpr (std::is_floating_point_v<T>) {
        printf("%f", static_cast<double>(value));
    } else if constexpr (std::is_pointer_v<T>) {
        // Use reinterpret_cast to handle volatile and other qualifiers
        printf("%p", reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(value)));
    } else if constexpr (std::is_same_v<T, bool>) {
        printf("%s", value ? "true" : "false");
    } else if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            printf("%ld", static_cast<long>(value));
        } else {
            printf("%lu", static_cast<unsigned long>(value));
        }
    } else {
        printf("%p", reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&value)));
    }
}


// C++20: Recursively print all arguments with comma separation
template<typename T, typename... Args>
inline void print_values(const T& first, const Args&... rest) {
    print_value(first);
    if constexpr (sizeof...(Args) > 0) {
        printf(", ");
        print_values(rest...);
    }
}

template<typename T>
inline void print_values(const T& last) {
    print_value(last);
}

// C++20: Print all arguments
template<typename... Args>
inline void print_all_args([[maybe_unused]] const Args&... args) {
    if constexpr (sizeof...(Args) > 0) {
        print_values(args...);
    }
}
#endif

//   This function takes care of generating automatic thunks for the functions
// we plan on exporting to the guest application, so that the functions we export
// are on the correct ABI and using the correct types. 
template<typename D, typename R, class... Args>
struct ThunkImpl
{
    //   Call helpers, the brace-initialization hack is made necessary to ensure
    // that the argument evaluation order is consistent, otherwise on GCC the
    // arguments are handled right to left instead. 
    struct BraceCall
    {
        R ret;
        template <typename... Arg>
        inline BraceCall(Arg... args) : ret( D::template bridge_impl<Args...>(args...) ) { };
    };

    struct BraceCallVoid
    {
        template <typename... Arg>
        inline BraceCallVoid(Arg... args) { D::template bridge_impl<Args...>(args...); };
    };

    template <typename V>
    static inline void flush_me(V v)
    {
        //   Some applications have feof(...) declared as a macro that accesses
        // the _flags component directly, this forces us to wrap FILE* on a container
        // that we control so we can emulate this behavior.
        if constexpr (std::is_same_v<V, FILE*>) {
            BIONIC_FILE *bf = (BIONIC_FILE*)v;
            FILE *f = (FILE*)bf->_cookie;
            bf->_flags = (feof(f) << 5) | (ferror(f) << 6);
        }
    }

    // Convert different types
    template <typename V>
    static inline V get_from_bionic(V v)
    {
        if constexpr (std::is_same_v<V, FILE*>) {
#ifndef NDEBUG
            // Only enabled on DEBUG builds so we can have some information on
            // odd behavior.
            if (v == NULL) {
                fatal_error("Attempted to read NULL file pointer.\n");
            }
#endif
            return (FILE*)((BIONIC_FILE*)v)->_cookie;
        } else {
            return v;
        }
    }

    ABI_ATTR static inline R bridge(Args... args)
    {
        static_assert(std::is_same_v<R, FILE*> != true, "Return type can't be a FILE pointer, implement me instead.");

        constexpr bool needs_flush = is_same_function<D::orig, fread>::value;
        if constexpr (std::is_void_v<R>) {
            BraceCallVoid{get_from_bionic(args)...};
            if constexpr (needs_flush)
                (flush_me(args),...);
        } else {
            R ret = BraceCall{get_from_bionic(args)...}.ret;
            if constexpr (needs_flush)
                (flush_me(args),...);

            return ret;
        }
    }

    // Conditions that are necessary for a function to need thunking
    static constexpr bool needs_thunking =
#if !defined(NO_ABI_ATTR)
        has_float_arg<R, Args...>::value ||
#endif
        has_FILEp_arg<R, Args...>::value;
};

// Dig out noexception
template<typename D, typename R, class... Args, bool EX>
struct ThunkImpl<D, R(*)(Args...) noexcept(EX)>: ThunkImpl<D, R, Args...>
{

};

#pragma GCC diagnostic ignored "-Wignored-attributes"
template<typename Callable>
using return_type_of_t = 
    typename decltype(std::function{std::declval<Callable>()})::result_type;

// Thunk class specializes into ThunkImpl to dig down into the function prototype
template<auto F>
struct Thunk : ThunkImpl<Thunk<F>, std::remove_cvref_t<decltype(F)>>
{
#if defined(WANTS_TRACE)
    static inline const char *trace = NULL;
#endif
    static constexpr auto orig = F;
    template<typename... Args>

    inline static auto bridge_impl(Args... args)
    {
#if defined(WANTS_TRACE)
        if constexpr (std::is_void_v<return_type_of_t<decltype(F)>>) {
            if (trace) {
                printf("TRACE %s(", trace);
                print_all_args(args...);
                printf(")\n");
            }
            F(args...);
        } else {
            auto r = F(args...);
            if (trace) {
                printf("TRACE %s(", trace);
                print_all_args(args...);
                printf(") -> ");
                print_value(r);
                printf("\n");
            }
            return r;
        }
#else
        return F(args...);
#endif
    }
};

template<typename testType>
struct is_function_pointer
{
    static const bool value =
        std::is_pointer<testType>::value ?
        std::is_function<typename std::remove_pointer<testType>::type>::value :
        false;
};

//   We need thunks to convert ABIs for armhf targets, unpack some structures,
// etc, but we don't want to thunk needlessly, as to not incur the overhead.
//   This function uses some metaprogramming tricks to dissect the function
// prototype, deciding if this needs or not thunking and then either returns
// the original pointer, or the thunked variant.
template <auto F, class T = Thunk<F>>
constexpr uintptr_t select_either(const char *f)
{
#if defined(WANTS_TRACE)
    if ((void*)F != (void*)memcpy && (void*)F != (void*)memset)
        T::trace = f;
    return (uintptr_t)T::bridge;
#else
    if constexpr (T::needs_thunking)
        return (uintptr_t)T::bridge;
    else
        return (uintptr_t)T::orig;
#endif
}

// Non-function or prototype-less symbols 
template <auto F, typename std::enable_if<
            !is_function_pointer<decltype(F)>::value,
            bool>::type = true>
constexpr uintptr_t select_either(const char *f)
{
    return (uintptr_t)F;
}

#define THUNK_DIRECT(f) {#f,select_either<&f>(#f)}
#define THUNK_SPECIFIC(str, func) {str,select_either<&func>(str)}
#define THUNK_TRACE(str, func) {str,select_either<&func>(str)}
#define NO_THUNK(str, func) {str, func}