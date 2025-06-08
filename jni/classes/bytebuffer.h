#pragma once
#include "jni.h"
#include "jni_internals.h"

class ByteBuffer : public Object {
public:
    static Class clazz;
    Class *_getClass() { return &clazz; }
    ByteBuffer(void* address, long capacity);
    int capacity();
    void* address();
private:
    int buf_capacity;
    void* buffer;
};