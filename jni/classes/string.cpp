#include <string.h>
#include <vector>

#include "jni.h"
#include "jni_internals.h"

Class String::clazz = {
    .classpath = "java/lang/String",
    .classname = "String",
    .managed_methods = {NULL},
    .fields = {NULL},
    .instance_size = sizeof(String)
};

String::String(char *str) {
    this->str = str;
}

String::String(const char *str) {
    this->str = strdup(str);
}

static const int registered = ClassRegistry::register_class(String::clazz);
