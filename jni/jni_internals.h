#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <tuple>
#include <functional>
#include <type_traits>
#include "jni.h"

// Forwards definitions
struct ManagedMethod;
struct NativeMethod;
struct FieldId;
struct Class;
struct Object;
struct ArrayObject;
struct String;

// Automatic generator for function dispatch
template <auto F>
struct dispatch
{
    // The following nested structure does two things:
    // 1: Unwraps details about the function (e.g. argument type list,
    //    return type) with the template.
    // 2: Implements the dispatcher as needed (will extract from va_list and push)
    //    into the function call that is being wrapped, taking care to also return
    //    the value if applicable.
    template<typename S>
    struct unwrap;

    template<typename R, typename... Args>
    struct unwrap<R(Args...)>
    {
        // brace-initialization is necessary to ensure the parameters are
        // evaluated in left-to-right order on gcc...
        struct BraceCall
        {
            R ret;
            template <typename... Arg>
            BraceCall(Arg... args) : ret( F(args...) ) { };
        };

        struct BraceCallVoid
        {
            template <typename... Arg>
            BraceCallVoid(Arg... args) { F(args...); };
        };
        
        public:
        static R dispatch_v(va_list va)
        {
            if constexpr (std::is_same_v<R, void>) {
                BraceCallVoid{(Args)va_arg(va, Args)...};
            } else {
                return BraceCall{(Args)va_arg(va, Args)...}.ret;
            }
        }

        static R dispatch_a(jvalue *arr)
        {
            // Fixes unsequenced nonsense...
            auto get = [&]() { return arr++; };

            if constexpr (std::is_same_v<R, void>) {
                BraceCallVoid{(*((Args*)get()))...};
            } else {
                return BraceCall{*((Args*)get())...}.ret;
            }
        }
    };

    // Unwrap the function
    using sig = unwrap<typename std::remove_pointer<decltype(F)>::type>;

    // Expose the dispatch function :)
    static constexpr auto vargs = sig::dispatch_v;
    static constexpr auto aargs = sig::dispatch_a;
};

struct Class {
    const char *classpath;
    const char *classname;
    const ManagedMethod *managed_methods;
    const NativeMethod *native_methods;
    const FieldId *fields;
    jsize instance_size;
};

struct Object {
    Class *clazz;
    Object(Class *clz) : clazz(clz) { }
    /* ... */
};

struct ArrayObject : Object {
    Class *instance_clazz;
    jsize count;
    jsize element_size;
    void *elements;
};

struct ManagedMethod {
    Class *clazz;
    const char *name;
    const char *signature;
    const void *addr_variadic; // For <...> and <va_list> 
    const void *addr_array; // For arrays

    template <auto *F>
    static const ManagedMethod Register(Class &clazz, const char *name, const char *signature)
    {
        using disp = dispatch<F>;
        return ManagedMethod {
            .clazz = &clazz,
            .name = name,
            .signature = signature,
            .addr_variadic = (void*)disp::vargs,
            .addr_array = (void*)disp::aargs,
        };
    }
};

struct NativeMethod {
    Class *clazz;
    const char *name;
    const char *soname;
    void **ptr;
};

struct FieldId {
    Class *clazz; // Keep a reference back to who owns this
    const char *name;
    const char *signature;
    uintptr_t offset; // Direct address for static fields, offsets for instance fields
    int is_static;
};

class ClassRegistry {
public:
    static std::vector<const Class*> &get_class_registry();
    static int register_class(const Class &clazz);
};

class String : Object {
public:
    char *str;
    String(char *str);
    String(const char *str);
    
    friend String* operator&(_jstring& jstr) { return (String*)&jstr; };
};

extern "C" {
    extern void jni_resolve_native(struct so_module *so);
}

JNIEnv *JNIEXT_GetCurrentEnv();

#define REGISTER_STATIC_FIELD(clz, field) \
    {.clazz = &clz::clazz, .name = #field, .offset = (uintptr_t)&clz::field, .is_static = 1}

#define REGISTER_FIELD(clz, field) \
    {.clazz = &clz::clazz, .name = #field, .offset = (uintptr_t)&(((clz*)0x0)->field), .is_static = 0}
    