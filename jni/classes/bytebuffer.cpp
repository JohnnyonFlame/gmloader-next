#include <vector>
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include "jni.h"
#include "jni_internals.h"
#include "bytebuffer.h"

Class ByteBuffer::clazz = {
    .classpath = "java/nio/ByteBuffer",
    .classname = "ByteBuffer",
    .managed_methods = {NULL},
    .fields = {NULL},
    .instance_size = sizeof(ByteBuffer)
};

ByteBuffer::ByteBuffer(void* pAddress, long pCapacity) {
    this->buf_capacity=pCapacity;
    this->buffer=pAddress;
}

int ByteBuffer::capacity(){return this->buf_capacity;}
void* ByteBuffer::address(){return this->buffer;}

static const int registered = ClassRegistry::register_class(ByteBuffer::clazz);
