/* Incomplete, and not 100% accurate C implementation of JNI */
/* See included classes to understand how to interface with this */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <iterator>
#include <algorithm>

#include "platform.h"
#include "jni.h"
#include "jni_internals.h"

extern const JNIInvokeInterface jvm_funcs;
extern const JNINativeInterface jnienv_funcs;
static JavaVM *jvm_global = NULL;

#define free(...)

ABI_ATTR static jobjectArray iface_NewObjectArray(JNIEnv *env, jsize len, jclass clz, jobject init)
{
    Class *clazz = (Class*)clz;
    Object *first_elem = (Object*)init;
    if (first_elem == NULL)
        return NULL;

    ArrayObject *arr = (ArrayObject*)calloc(1, sizeof(ArrayObject));
    if (arr == NULL)
        return NULL;

    arr->instance_clazz = clazz;
    arr->count = len;
    arr->element_size = clazz->instance_size;
    arr->elements = (Object*)malloc(clazz->instance_size * len);

    if (arr->elements == NULL) {
        free(arr);
        return NULL;
    }

    for (jsize i = 0; i < len; i++) {
        uintptr_t offs = (uintptr_t)arr->elements + (arr->element_size * i);
        memcpy((void*)offs, first_elem, arr->element_size);
    }

    return (jobjectArray)arr;
}

ABI_ATTR static jint iface_GetVersion(JNIEnv *env)
{
    return 10;
}

ABI_ATTR static jclass iface_DefineClass(JNIEnv *env, const char *name, jobject loader,
    const jbyte* buf, jsize bufLen)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jclass iface_FindClass(JNIEnv *env, const char* name)
{
    auto registry = ClassRegistry::get_class_registry();
    auto it = std::find_if(registry.begin(), registry.end(),
        [&](auto *item) { return strcmp(name, item->classpath) == 0; });
    if (it == registry.end())
        return NULL;

    return (jclass)*it;
}

ABI_ATTR static jclass iface_GetSuperclass(JNIEnv *env, jclass jclazz)
{
    /* No inheritance here */
    return jclazz;
}

ABI_ATTR static jboolean iface_IsAssignableFrom(JNIEnv *env, jclass jclazz1, jclass jclazz2)
{
    WARN_STUB;
    return JNI_FALSE;
}

ABI_ATTR static jmethodID iface_FromReflectedMethod(JNIEnv *env, jobject jmethod)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jfieldID iface_FromReflectedField(JNIEnv *env, jobject jfield)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jobject iface_ToReflectedMethod(JNIEnv *env, jclass jcls, jmethodID methodID, jboolean isStatic)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jobject iface_ToReflectedField(JNIEnv *env, jclass jcls, jfieldID fieldID, jboolean isStatic)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jint iface_Throw(JNIEnv *env, jthrowable jobj)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jint iface_ThrowNew(JNIEnv *env, jclass jclazz, const char* message)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jthrowable iface_ExceptionOccurred(JNIEnv *env)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static void iface_ExceptionDescribe(JNIEnv *env)
{
    WARN_STUB;
}

ABI_ATTR static void iface_ExceptionClear(JNIEnv *env)
{
    WARN_STUB;
}

ABI_ATTR static void iface_FatalError(JNIEnv *env, const char* msg)
{
    WARN_STUB;
}

ABI_ATTR static jint iface_PushLocalFrame(JNIEnv *env, jint capacity)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jobject iface_PopLocalFrame(JNIEnv *env, jobject jresult)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jobject iface_NewGlobalRef(JNIEnv *env, jobject jobj)
{
    return jobj;
}

ABI_ATTR static void iface_DeleteGlobalRef(JNIEnv *env, jobject jglobalRef)
{
    ;
}

ABI_ATTR static jobject iface_NewLocalRef(JNIEnv *env, jobject jobj)
{
    return jobj;
}

ABI_ATTR static void iface_DeleteLocalRef(JNIEnv *env, jobject jlocalRef)
{
    ;
}

ABI_ATTR static jint iface_EnsureLocalCapacity(JNIEnv *env, jint capacity)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jboolean iface_IsSameObject(JNIEnv *env, jobject jref1, jobject jref2)
{
    WARN_STUB;
    return JNI_FALSE;
}

ABI_ATTR static jobject iface_AllocObject(JNIEnv *env, jclass jclazz)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jobject iface_NewObject(JNIEnv *env, jclass jclazz, jmethodID methodID, ...)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jobject iface_NewObjectV(JNIEnv *env, jclass jclazz, jmethodID methodID, va_list args)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jobject iface_NewObjectA(JNIEnv *env, jclass jclazz, jmethodID methodID, const jvalue* args)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jclass iface_GetObjectClass(JNIEnv *env, jobject jobj)
{
    Object *obj = (Object*)jobj;
    if (obj == NULL)
        return NULL;

    return (jclass)obj->clazz;
}

ABI_ATTR static jboolean iface_IsInstanceOf(JNIEnv *env, jobject jobj, jclass jclazz)
{
    WARN_STUB;
    return JNI_FALSE;
}

ABI_ATTR static jmethodID iface_GetMethodID(JNIEnv *env, jclass clazz, const char* name, const char* sig)
{
    Class *clz = (Class*)clazz;
    if (clz) {
        for (int i = 0; clz->managed_methods[i].name; i++) {
            if ((strcmp(name, clz->managed_methods[i].name)      == 0) &&
                (strcmp(sig,  clz->managed_methods[i].signature) == 0)) {
                return (jmethodID)&clz->managed_methods[i];
            }
        }

        warning("Class %s does not have method %s.\n", clz->classname, name);
    } else {
        warning("Could not look up method %s, NULL class.\n", name);
    }

    return NULL;
}

static jfieldID GetFieldIdGeneric(JNIEnv *env, jclass clazz, const char* name, const char* sig)
{
    Class *clz = (Class*)clazz;
    if (clz != NULL) {
        for (int i = 0; clz->fields[i].name; i++) {
            if (strcmp(name, clz->fields[i].name) == 0) {
                return (jfieldID)&clz->fields[i];
            }
        }

        warning("Class %s does not have field %s.\n", clz->classname, name);
    } else {
        warning("Could not look up field %s, NULL class.\n", name);
    }
   
    return NULL;
}

ABI_ATTR static jfieldID iface_GetFieldID(JNIEnv *env, jclass clazz, const char* name, const char* sig)
{
    FieldId *field = (FieldId*)GetFieldIdGeneric(env, clazz, name, sig);
    if (field == NULL || field->is_static == 1)
        return NULL;
    
    return (jfieldID)field;
}

ABI_ATTR static jmethodID iface_GetStaticMethodID(JNIEnv *env, jclass clazz, const char* name, const char* sig)
{
    return iface_GetMethodID(env, clazz, name, sig);
    //WARN_STUB;
}

ABI_ATTR static jfieldID iface_GetStaticFieldID(JNIEnv *env, jclass clazz, const char* name, const char* sig)
{
    FieldId *field = (FieldId*)GetFieldIdGeneric(env, clazz, name, sig);
    if (field == NULL || field->is_static == 0)
        return NULL;

    return (jfieldID)field;
}

ABI_ATTR static jstring iface_NewString(JNIEnv *env, const jchar* unicodeChars, jsize len)
{
    String *str = new String((const char *)unicodeChars);
    return (jstring)str;
}

ABI_ATTR static jsize iface_GetStringLength(JNIEnv *env, jstring jstr)
{
    String *str = (String*)jstr;
    return strlen(str->str);
}

ABI_ATTR static const jchar* iface_GetStringChars(JNIEnv *env, jstring jstr, jboolean* isCopy)
{
    String *str = (String*)jstr;
    if (isCopy)
        *isCopy = JNI_TRUE;
   
    if (str->str == NULL)
        return NULL;
   
    return (jchar *)strdup(str->str);
}

ABI_ATTR static void iface_ReleaseStringChars(JNIEnv *env, jstring jstr, const jchar* chars)
{
    free((void*)chars);
}

ABI_ATTR static jstring iface_NewStringUTF(JNIEnv *env, const char* bytes)
{
    String *str = new String(bytes);
    return (jstring)str;
}

ABI_ATTR static jsize iface_GetStringUTFLength(JNIEnv *env, jstring jstr)
{
    String *str = (String*)jstr;
    return strlen(str->str);
}

ABI_ATTR static const char* iface_GetStringUTFChars(JNIEnv *env, jstring jstr, jboolean* isCopy)
{
    String *str = (String*)jstr;
    if (isCopy)
        *isCopy = JNI_TRUE;

    return strdup(jstr ? str->str : "(nil)");
}

ABI_ATTR static void iface_ReleaseStringUTFChars(JNIEnv *env, jstring jstr, const char* utf)
{
    free((void*)utf);
}

ABI_ATTR static jsize iface_GetArrayLength(JNIEnv *env, jarray jarr)
{
    ArrayObject* arr = (ArrayObject*)jarr;
    return arr->count;
}

ABI_ATTR static jobject iface_GetObjectArrayElement(JNIEnv *env, jobjectArray jarr, jsize index)
{
    ArrayObject* arr = (ArrayObject*)jarr;
    uintptr_t offs = (uintptr_t)arr->elements + (index * arr->element_size);
    return (jobject)(offs);
}

ABI_ATTR static void iface_SetObjectArrayElement(JNIEnv *env, jobjectArray jarr, jsize index, jobject jobj)
{
    ArrayObject* arr = (ArrayObject*)jarr;
    if (jarr == NULL)
        return;

    uintptr_t offs = (intptr_t)arr->elements + (arr->element_size * index);
    memcpy((void*)offs, (void*)jobj, arr->element_size);
}

ABI_ATTR static jint iface_RegisterNatives(JNIEnv *env, jclass jclazz,
    const JNINativeMethod* methods, jint nMethods)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jint iface_UnregisterNatives(JNIEnv *env, jclass jclazz)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jint iface_MonitorEnter(JNIEnv *env, jobject jobj)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jint iface_MonitorExit(JNIEnv *env, jobject jobj)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jint iface_GetJavaVM(JNIEnv *env, JavaVM** vm)
{
    *vm = jvm_global;
    return 0;
}

ABI_ATTR static void iface_GetStringRegion(JNIEnv *env, jstring jstr, jsize start, jsize len, jchar* buf)
{
    WARN_STUB;
}

ABI_ATTR static void iface_GetStringUTFRegion(JNIEnv *env, jstring jstr, jsize start, jsize len, char* buf)
{
    WARN_STUB;
}

ABI_ATTR static void* iface_GetPrimitiveArrayCritical(JNIEnv *env, jarray jarr, jboolean* isCopy)
{
    //WARN_STUB;
    return NULL;
}

ABI_ATTR static void iface_ReleasePrimitiveArrayCritical(JNIEnv *env, jarray jarr, void* carray, jint mode)
{
    WARN_STUB;
}

ABI_ATTR static const jchar* iface_GetStringCritical(JNIEnv *env, jstring jstr, jboolean* isCopy)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static void iface_ReleaseStringCritical(JNIEnv *env, jstring jstr, const jchar* carray)
{
    WARN_STUB;
}

ABI_ATTR static jweak iface_NewWeakGlobalRef(JNIEnv *env, jobject jobj)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static void iface_DeleteWeakGlobalRef(JNIEnv *env, jweak wref)
{
    WARN_STUB;
}

ABI_ATTR static jboolean iface_ExceptionCheck(JNIEnv *env)
{
    WARN_STUB;
    return JNI_FALSE;
}

ABI_ATTR static jobjectRefType iface_GetObjectRefType(JNIEnv *env, jobject jobj)
{
    WARN_STUB;
    return JNIInvalidRefType;
}

ABI_ATTR static jobject iface_NewDirectByteBuffer(JNIEnv *env, void* address, jlong capacity)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static void* iface_GetDirectBufferAddress(JNIEnv *env, jobject jbuf)
{
    WARN_STUB;
    return NULL;
}

ABI_ATTR static jlong iface_GetDirectBufferCapacity(JNIEnv *env, jobject jbuf)
{
    WARN_STUB;
    return 0L;
}

template <typename T>
ABI_ATTR ABI_ATTR static T iface_CallMethod(JNIEnv *env, jobject obj, jmethodID meth, ...)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    va_list va;
    va_start(va, meth);
    T (*dispatch)(JNIEnv *, jobject, va_list) = (decltype(dispatch))method->addr_variadic;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, obj, va);
        va_end(va);
    } else {
        T ret = dispatch(env, obj, va);
        va_end(va);
        return ret;
    }
}

template <typename T>
ABI_ATTR static T iface_CallMethodV(JNIEnv *env, jobject obj, jmethodID meth, va_list va)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    T (*dispatch)(JNIEnv *, jobject, va_list) = (decltype(dispatch))method->addr_variadic;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, obj, va);
    } else {
        T ret = dispatch(env, obj, va);
        return ret;
    }
}

template <typename T> 
ABI_ATTR static T iface_CallMethodA(JNIEnv *env, jobject obj, jmethodID meth, const jvalue *val)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    T (*dispatch)(JNIEnv *, jobject, const jvalue *) = (decltype(dispatch))method->addr_array;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, obj, val);
    } else {
        return dispatch(env, obj, val);
    }
}

template <typename T>
ABI_ATTR static T iface_CallStaticMethod(JNIEnv *env, jclass jclz, jmethodID meth, ...)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    va_list va;
    va_start(va, meth);
    T (*dispatch)(JNIEnv *, jclass, va_list) = (decltype(dispatch))method->addr_variadic;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, jclz, va);
        va_end(va);
    } else {
        T ret = dispatch(env, jclz, va);
        va_end(va);
        return ret;
    }
}

template <typename T>
ABI_ATTR static T iface_CallStaticMethodV(JNIEnv *env, jclass jclz, jmethodID meth, va_list va)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    T (*dispatch)(JNIEnv *, jclass, va_list) = (decltype(dispatch))method->addr_variadic;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, jclz, va);
    } else {
        T ret = dispatch(env, jclz, va);
        return ret;
    }
}

template <typename T> 
ABI_ATTR static T iface_CallStaticMethodA(JNIEnv *env, jclass clz, jmethodID meth, const jvalue *val)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    T (*dispatch)(JNIEnv *, jclass, const jvalue *) = (decltype(dispatch))method->addr_array;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, clz, val);
    } else {
        return dispatch(env, clz, val);
    }
}

template <typename T>
ABI_ATTR static T iface_CallNonVirtualMethod(JNIEnv *env, jobject obj, jclass jclazz, jmethodID meth, ...)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    va_list va;
    va_start(va, meth);
    T (*dispatch)(JNIEnv *, jobject, jclass, va_list) = (decltype(dispatch))method->addr_variadic;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, obj, jclazz, va);
        va_end(va);
    } else {
        T ret = dispatch(env, obj, jclazz, va);
        va_end(va);
        return ret;
    }
}

template <typename T>
ABI_ATTR static T iface_CallNonVirtualMethodV(JNIEnv *env, jobject obj, jclass jclazz, jmethodID meth, va_list va)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    T (*dispatch)(JNIEnv *, jobject, jclass, va_list) = (decltype(dispatch))method->addr_variadic;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, obj, jclazz, va);
    } else {
        return dispatch(env, obj, jclazz, va);
    }
}

template <typename T> 
ABI_ATTR static T iface_CallNonVirtualMethodA(JNIEnv *env, jobject obj, jclass jclazz, jmethodID meth, const jvalue *val)
{
    ManagedMethod *method = (ManagedMethod*)meth;
    if (!method || !method->addr_variadic) {
        if constexpr (std::is_same_v<T, void>)
            return;
        else
            return (T)0;
    }

    T (*dispatch)(JNIEnv *, jobject, jclass, const jvalue *) = (decltype(dispatch))method->addr_array;
    if constexpr (std::is_same_v<T, void>) {
        dispatch(env, obj, jclazz, val);
    } else {
        return dispatch(env, obj, jclazz, val);
    }
}

template <typename T>
ABI_ATTR static T iface_GetField(JNIEnv *env, jobject obj, jfieldID fieldID)
{
    FieldId *f = (FieldId*)fieldID;
    if (f == NULL || f->is_static == 1 || obj == NULL)
        return (T)0;

    T *offs = (T*)((intptr_t)obj + f->offset);

    return *offs;
}

template <typename T>
ABI_ATTR static void iface_SetField(JNIEnv *env, jobject obj, jfieldID fieldID, T value)
{
    FieldId *f = (FieldId*)fieldID;
    if (f == NULL || f->is_static == 1 || obj == NULL)
        return;

    T *offs = (T*)((intptr_t)obj + f->offset);

    *offs = value;
}

template <typename T>
ABI_ATTR static T iface_GetStaticField(JNIEnv *env, jclass clz, jfieldID fieldID)
{
    FieldId *f = (FieldId*)fieldID;
    if (f == NULL || f->is_static == 0 || clz == NULL)
        return (T)0;

    T offs = (T)(f->offset);
    return offs;
}

template <typename T>
ABI_ATTR static void iface_SetStaticField(JNIEnv *env, jclass clz, jfieldID fieldID, T value)
{
    FieldId *f = (FieldId*)fieldID;
    if (f == NULL || f->is_static == 0 || clz == NULL)
        return;

    T *offs = (T*)f->offset;
    *offs = value;
}

template <typename T>
ABI_ATTR static T iface_NewArray(JNIEnv *env, jsize length)
{
    jsize t = 4;
    if constexpr (std::is_same_v<T, jbooleanArray>) t = sizeof(jboolean);
    if constexpr (std::is_same_v<T, jbyteArray>) t = sizeof(jbyte);
    if constexpr (std::is_same_v<T, jcharArray>) t = sizeof(jchar);
    if constexpr (std::is_same_v<T, jshortArray>) t = sizeof(jshort);
    if constexpr (std::is_same_v<T, jintArray>) t = sizeof(jint);
    if constexpr (std::is_same_v<T, jlongArray>) t = sizeof(jlong);
    if constexpr (std::is_same_v<T, jfloatArray>) t = sizeof(jfloat);
    if constexpr (std::is_same_v<T, jdoubleArray>) t = sizeof(jdouble);

    ArrayObject *array = (ArrayObject *)calloc(1, sizeof(ArrayObject)); 
    array->clazz = NULL; // TODO
    array->count = length;
    array->element_size = t;
    array->elements = calloc(length, t);

    return (T)array;
}

template <typename T, typename Tarr>
ABI_ATTR static T * iface_GetArrayElements(JNIEnv *env, Tarr arr, jboolean *isCopy)
{
    ArrayObject *array = (ArrayObject*)arr;
    if (array == NULL)
        return NULL;

    if (isCopy)
        *isCopy = JNI_TRUE;
    
    T *elements = (T*)malloc(array->count * array->element_size);
    memcpy((void*)elements, array->elements, array->count * array->element_size);
    return elements;
}

template <typename T, typename Tarr>
ABI_ATTR static void iface_ReleaseArrayElements(JNIEnv*, Tarr arr, T *els, jint mode)
{
    ArrayObject *array = (ArrayObject*)arr;
    if (array == NULL)
        return;

    if (mode != JNI_ABORT)
        memcpy(array->elements, els, array->count * array->element_size);
    
    if (mode != JNI_COMMIT)
        free(els);
}

template <typename T, typename Tarr>
ABI_ATTR static void iface_GetArrayRegion(JNIEnv *env, Tarr arr, jsize start, jsize len, T *buf)
{
    ArrayObject *array = (ArrayObject*)arr;
    if (array == NULL)
        return;

    if (len > array->count - start)
        return;

    uintptr_t offs = (uintptr_t)array->elements + (start * array->element_size);
    memcpy(buf, (void*)offs, len * array->element_size);
}

template <typename T, typename Tarr>
ABI_ATTR void iface_SetArrayRegion(JNIEnv *env, Tarr arr, jsize start, jsize len, const T *buf)
{
    ArrayObject *array = (ArrayObject*)arr;
    if (array == NULL)
        return;

    if (len > array->count - start)
        return;

    uintptr_t offs = (uintptr_t)array->elements + (start * array->element_size);
    memcpy((void*)offs, (void*)buf, len * array->element_size);
}

const JNINativeInterface jnienv_funcs = {
    .reserved0 = NULL,
    .GetVersion = &iface_GetVersion,
    .DefineClass = &iface_DefineClass,
    .FindClass = &iface_FindClass,

    .FromReflectedMethod = &iface_FromReflectedMethod,
    .FromReflectedField = &iface_FromReflectedField,
    /* spec doesn't show jboolean parameter */
    .ToReflectedMethod = &iface_ToReflectedMethod,
    .GetSuperclass = &iface_GetSuperclass,
    .IsAssignableFrom = &iface_IsAssignableFrom,

    /* spec doesn't show jboolean parameter */
    .ToReflectedField = &iface_ToReflectedField,
    .Throw = &iface_Throw,
    .ThrowNew = &iface_ThrowNew,
    .ExceptionOccurred = &iface_ExceptionOccurred,
    .ExceptionDescribe = &iface_ExceptionDescribe,
    .ExceptionClear = &iface_ExceptionClear,
    .FatalError = &iface_FatalError,
    .PushLocalFrame = &iface_PushLocalFrame,
    .PopLocalFrame = &iface_PopLocalFrame,
    .NewGlobalRef = &iface_NewGlobalRef,
    .DeleteGlobalRef = &iface_DeleteGlobalRef,
    .DeleteLocalRef = &iface_DeleteLocalRef,
    .IsSameObject = &iface_IsSameObject,
    .NewLocalRef = &iface_NewLocalRef,
    .EnsureLocalCapacity = &iface_EnsureLocalCapacity,
    .AllocObject = &iface_AllocObject,
    .NewObject = &iface_NewObject,
    .NewObjectV = &iface_NewObjectV,
    .NewObjectA = &iface_NewObjectA,
    .GetObjectClass = &iface_GetObjectClass,
    .IsInstanceOf = &iface_IsInstanceOf,
    .GetMethodID = &iface_GetMethodID,

    .CallObjectMethod = &iface_CallMethod,
    .CallObjectMethodV = &iface_CallMethodV,
    .CallObjectMethodA = &iface_CallMethodA,
    .CallBooleanMethod = &iface_CallMethod,
    .CallBooleanMethodV = &iface_CallMethodV,
    .CallBooleanMethodA = &iface_CallMethodA,
    .CallByteMethod = &iface_CallMethod,
    .CallByteMethodV = &iface_CallMethodV,
    .CallByteMethodA = &iface_CallMethodA,
    .CallCharMethod = &iface_CallMethod,
    .CallCharMethodV = &iface_CallMethodV,
    .CallCharMethodA = &iface_CallMethodA,
    .CallShortMethod = &iface_CallMethod,
    .CallShortMethodV = &iface_CallMethodV,
    .CallShortMethodA = &iface_CallMethodA,
    .CallIntMethod = &iface_CallMethod,
    .CallIntMethodV = &iface_CallMethodV,
    .CallIntMethodA = &iface_CallMethodA,
    .CallLongMethod = &iface_CallMethod,
    .CallLongMethodV = &iface_CallMethodV,
    .CallLongMethodA = &iface_CallMethodA,
    .CallFloatMethod = &iface_CallMethod,
    .CallFloatMethodV = &iface_CallMethodV,
    .CallFloatMethodA = &iface_CallMethodA,
    .CallDoubleMethod = &iface_CallMethod,
    .CallDoubleMethodV = &iface_CallMethodV,
    .CallDoubleMethodA = &iface_CallMethodA,
    .CallVoidMethod = &iface_CallMethod,
    .CallVoidMethodV = &iface_CallMethodV,
    .CallVoidMethodA = &iface_CallMethodA,
    .CallNonvirtualObjectMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualObjectMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualObjectMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualBooleanMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualBooleanMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualBooleanMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualByteMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualByteMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualByteMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualCharMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualCharMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualCharMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualShortMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualShortMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualShortMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualIntMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualIntMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualIntMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualLongMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualLongMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualLongMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualFloatMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualFloatMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualFloatMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualDoubleMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualDoubleMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualDoubleMethodA = &iface_CallNonVirtualMethodA,
    .CallNonvirtualVoidMethod = &iface_CallNonVirtualMethod,
    .CallNonvirtualVoidMethodV = &iface_CallNonVirtualMethodV,
    .CallNonvirtualVoidMethodA = &iface_CallNonVirtualMethodA,

    .GetFieldID = &iface_GetFieldID,
    .GetObjectField = &iface_GetField,
    .GetBooleanField = &iface_GetField,
    .GetByteField = &iface_GetField,
    .GetCharField = &iface_GetField,
    .GetShortField = &iface_GetField,
    .GetIntField = &iface_GetField,
    .GetLongField = &iface_GetField,
    .GetFloatField = &iface_GetField,
    .GetDoubleField = &iface_GetField,
    .SetObjectField = &iface_SetField,
    .SetBooleanField = &iface_SetField,
    .SetByteField = &iface_SetField,
    .SetCharField = &iface_SetField,
    .SetShortField = &iface_SetField,
    .SetIntField = &iface_SetField,
    .SetLongField = &iface_SetField,
    .SetFloatField = &iface_SetField,
    .SetDoubleField = &iface_SetField,
    .GetStaticMethodID = &iface_GetStaticMethodID,
    .CallStaticObjectMethod = &iface_CallStaticMethod,
    .CallStaticObjectMethodV = &iface_CallStaticMethodV,
    .CallStaticObjectMethodA = &iface_CallStaticMethodA,
    .CallStaticBooleanMethod = &iface_CallStaticMethod,
    .CallStaticBooleanMethodV = &iface_CallStaticMethodV,
    .CallStaticBooleanMethodA = &iface_CallStaticMethodA,
    .CallStaticByteMethod = &iface_CallStaticMethod,
    .CallStaticByteMethodV = &iface_CallStaticMethodV,
    .CallStaticByteMethodA = &iface_CallStaticMethodA,
    .CallStaticCharMethod = &iface_CallStaticMethod,
    .CallStaticCharMethodV = &iface_CallStaticMethodV,
    .CallStaticCharMethodA = &iface_CallStaticMethodA,
    .CallStaticShortMethod = &iface_CallStaticMethod,
    .CallStaticShortMethodV = &iface_CallStaticMethodV,
    .CallStaticShortMethodA = &iface_CallStaticMethodA,
    .CallStaticIntMethod = &iface_CallStaticMethod,
    .CallStaticIntMethodV = &iface_CallStaticMethodV,
    .CallStaticIntMethodA = &iface_CallStaticMethodA,
    .CallStaticLongMethod = &iface_CallStaticMethod,
    .CallStaticLongMethodV = &iface_CallStaticMethodV,
    .CallStaticLongMethodA = &iface_CallStaticMethodA,
    .CallStaticFloatMethod = &iface_CallStaticMethod,
    .CallStaticFloatMethodV = &iface_CallStaticMethodV,
    .CallStaticFloatMethodA = &iface_CallStaticMethodA,
    .CallStaticDoubleMethod = &iface_CallStaticMethod,
    .CallStaticDoubleMethodV = &iface_CallStaticMethodV,
    .CallStaticDoubleMethodA = &iface_CallStaticMethodA,
    .CallStaticVoidMethod = &iface_CallStaticMethod,
    .CallStaticVoidMethodV = &iface_CallStaticMethodV,
    .CallStaticVoidMethodA = &iface_CallStaticMethodA,
    .GetStaticFieldID = &iface_GetStaticFieldID,
    .GetStaticObjectField = &iface_GetStaticField,
    .GetStaticBooleanField = &iface_GetStaticField,
    .GetStaticByteField = &iface_GetStaticField,
    .GetStaticCharField = &iface_GetStaticField,
    .GetStaticShortField = &iface_GetStaticField,
    .GetStaticIntField = &iface_GetStaticField,
    .GetStaticLongField = &iface_GetStaticField,
    .GetStaticFloatField = &iface_GetStaticField,
    .GetStaticDoubleField = &iface_GetStaticField,
    .SetStaticObjectField = &iface_SetStaticField,
    .SetStaticBooleanField = &iface_SetStaticField,
    .SetStaticByteField = &iface_SetStaticField,
    .SetStaticCharField = &iface_SetStaticField,
    .SetStaticShortField = &iface_SetStaticField,
    .SetStaticIntField = &iface_SetStaticField,
    .SetStaticLongField = &iface_SetStaticField,
    .SetStaticFloatField = &iface_SetStaticField,
    .SetStaticDoubleField = &iface_SetStaticField,

    .NewString = &iface_NewString,
    .GetStringLength = &iface_GetStringLength,
    .GetStringChars = &iface_GetStringChars,
    .ReleaseStringChars = &iface_ReleaseStringChars,
    .NewStringUTF = &iface_NewStringUTF,
    .GetStringUTFLength = &iface_GetStringUTFLength,

    .GetStringUTFChars = &iface_GetStringUTFChars,
    .ReleaseStringUTFChars = &iface_ReleaseStringUTFChars,
    .GetArrayLength = &iface_GetArrayLength,
    .NewObjectArray = &iface_NewObjectArray,
    .GetObjectArrayElement = &iface_GetObjectArrayElement,
    .SetObjectArrayElement = &iface_SetObjectArrayElement,

    .NewBooleanArray = &iface_NewArray,
    .NewByteArray = &iface_NewArray,
    .NewCharArray = &iface_NewArray,
    .NewShortArray = &iface_NewArray,
    .NewIntArray = &iface_NewArray,
    .NewLongArray = &iface_NewArray,
    .NewFloatArray = &iface_NewArray,
    .NewDoubleArray = &iface_NewArray,
    .GetBooleanArrayElements = &iface_GetArrayElements,
    .GetByteArrayElements = &iface_GetArrayElements,
    .GetCharArrayElements = &iface_GetArrayElements,
    .GetShortArrayElements = &iface_GetArrayElements,
    .GetIntArrayElements = &iface_GetArrayElements,
    .GetLongArrayElements = &iface_GetArrayElements,
    .GetFloatArrayElements = &iface_GetArrayElements,
    .GetDoubleArrayElements = &iface_GetArrayElements,
    .ReleaseBooleanArrayElements = &iface_ReleaseArrayElements,
    .ReleaseByteArrayElements = &iface_ReleaseArrayElements,
    .ReleaseCharArrayElements = &iface_ReleaseArrayElements,
    .ReleaseShortArrayElements = &iface_ReleaseArrayElements,
    .ReleaseIntArrayElements = &iface_ReleaseArrayElements,
    .ReleaseLongArrayElements = &iface_ReleaseArrayElements,
    .ReleaseFloatArrayElements = &iface_ReleaseArrayElements,
    .ReleaseDoubleArrayElements = &iface_ReleaseArrayElements,
    .GetBooleanArrayRegion = &iface_GetArrayRegion,
    .GetByteArrayRegion = &iface_GetArrayRegion,
    .GetCharArrayRegion = &iface_GetArrayRegion,
    .GetShortArrayRegion = &iface_GetArrayRegion,
    .GetIntArrayRegion = &iface_GetArrayRegion,
    .GetLongArrayRegion = &iface_GetArrayRegion,
    .GetFloatArrayRegion = &iface_GetArrayRegion,
    .GetDoubleArrayRegion = &iface_GetArrayRegion,

    .SetBooleanArrayRegion = &iface_SetArrayRegion,
    .SetByteArrayRegion = &iface_SetArrayRegion,
    .SetCharArrayRegion = &iface_SetArrayRegion,
    .SetShortArrayRegion = &iface_SetArrayRegion,
    .SetIntArrayRegion = &iface_SetArrayRegion,
    .SetLongArrayRegion = &iface_SetArrayRegion,
    .SetFloatArrayRegion = &iface_SetArrayRegion,
    .SetDoubleArrayRegion = &iface_SetArrayRegion,
    .RegisterNatives = &iface_RegisterNatives,
    .UnregisterNatives = &iface_UnregisterNatives,
    .MonitorEnter = &iface_MonitorEnter,
    .MonitorExit = &iface_MonitorExit,
    .GetJavaVM = &iface_GetJavaVM,
    .GetStringRegion = &iface_GetStringRegion,
    .GetStringUTFRegion = &iface_GetStringUTFRegion,
    .GetPrimitiveArrayCritical = &iface_GetPrimitiveArrayCritical,
    .ReleasePrimitiveArrayCritical = &iface_ReleasePrimitiveArrayCritical,
    .GetStringCritical = &iface_GetStringCritical,
    .ReleaseStringCritical = &iface_ReleaseStringCritical,
    .NewWeakGlobalRef = &iface_NewWeakGlobalRef,
    .DeleteWeakGlobalRef = &iface_DeleteWeakGlobalRef,
    .ExceptionCheck = &iface_ExceptionCheck,
    .NewDirectByteBuffer = &iface_NewDirectByteBuffer,
    .GetDirectBufferAddress = &iface_GetDirectBufferAddress,
    .GetDirectBufferCapacity = &iface_GetDirectBufferCapacity,

    /* added in JNI 1.6 */
    .GetObjectRefType = &iface_GetObjectRefType,
};

thread_local JNIEnv *tls_env = NULL;
ABI_ATTR static jint iface_AttachCurrentThread(JavaVM *vm, JNIEnv **p_env, void *thr_args)
{
    if (tls_env == NULL) {
        tls_env = (JNIEnv*)calloc(1, sizeof(JNIEnv));
        tls_env->functions = &jnienv_funcs;
    }

    *p_env = tls_env;
    return JNI_OK;
}

ABI_ATTR static jint iface_DestroyJavaVM(JavaVM *vm)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jint iface_DetachCurrentThread(JavaVM *vm)
{
    WARN_STUB;
    return 0;
}

ABI_ATTR static jint iface_GetEnv(JavaVM* vm, void **env, jint version)
{
    if (tls_env == NULL)
        return JNI_EDETACHED;

    *env = (void*)tls_env;
    return JNI_OK;
}

ABI_ATTR static jint iface_AttachCurrentThreadAsDaemon(JavaVM *vm, JNIEnv **env, void *thr_args)
{
    vm->AttachCurrentThread(env, thr_args);
    return JNI_OK;
}

const JNIInvokeInterface jvm_funcs = {
    .DestroyJavaVM = &iface_DestroyJavaVM,
    .AttachCurrentThread = &iface_AttachCurrentThread,
    .DetachCurrentThread = &iface_DetachCurrentThread,
    .GetEnv = &iface_GetEnv,
    .AttachCurrentThreadAsDaemon = &iface_AttachCurrentThreadAsDaemon
};

jint JNI_CreateJavaVM(JavaVM **p_vm, JNIEnv **p_env, void *vm_args)
{
    if (jvm_global == NULL) {
        jvm_global = (JavaVM*)calloc(1, sizeof(JavaVM));
        jvm_global->functions = &jvm_funcs;
    }

    jvm_global->AttachCurrentThread((JNIEnv**)p_env, NULL);
    *p_vm = jvm_global; 

    return JNI_OK;
}