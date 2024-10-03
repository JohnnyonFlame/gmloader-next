#include <vector>

#include "jni.h"
#include "jni_internals.h"
#include "os_Build.h"

String osBuild::MANUFACTURER("JohnnyonFlame");

const FieldId osBuildClassFields[] = {
    REGISTER_STATIC_FIELD(osBuild, MANUFACTURER),
    {NULL},
};

Class osBuild::clazz = {
    .classpath = "android/os/Build",
    .classname = "os_Build",
    .managed_methods = {NULL},
    .fields = osBuildClassFields,
    .instance_size = 0
};

static const int registered = ClassRegistry::register_class(osBuild::clazz);