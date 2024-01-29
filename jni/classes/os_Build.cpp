#include <vector>

#include "jni.h"
#include "jni_internals.h"
#include "os_Build.h"

String osBuildClass::MANUFACTURER("JohnnyonFlame");

const FieldId osBuildClassFields[] = {
    REGISTER_STATIC_FIELD(osBuildClass, MANUFACTURER),
    {NULL},
};

Class osBuildClass::clazz = {
    .classpath = "android/os/Build",
    .classname = "os_Build",
    .managed_methods = {NULL},
    .fields = osBuildClassFields,
    .instance_size = sizeof(String)
};

static const int registered = ClassRegistry::register_class(osBuildClass::clazz);