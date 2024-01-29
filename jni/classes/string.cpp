#include <string.h>
#include <vector>

#include "jni.h"
#include "jni_internals.h"

class StringClass {
public:
    static Class clazz;
    /* ... */
};

Class StringClass::clazz = {
    .classpath = "java/lang/String",
    .classname = "String",
    .managed_methods = {NULL},
    .fields = {NULL},
    .instance_size = sizeof(String)
};

String::String(char *str) : Object(&StringClass::clazz) {
    this->str = str;
}

String::String(const char *str) : Object(&StringClass::clazz) {
    this->str = strdup(str);
}

static const int registered = ClassRegistry::register_class(StringClass::clazz);