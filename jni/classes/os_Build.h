#pragma once
#include "jni.h"
#include "jni_internals.h"

class osBuild : public Object {
public:
    static Class clazz;
    Class *_getClass() { return &clazz; }
    static String MANUFACTURER;
};