#include <string.h>
#include <vector>
#include "platform.h"
#include "so_util.h"
#include "jni.h"
#include "jni_internals.h"

std::vector<const Class*> &ClassRegistry::get_class_registry() {
    static std::vector<const Class*> registry;
    return registry;
}

int ClassRegistry::register_class(const Class &clazz)
{
    auto &class_registry = get_class_registry();
    class_registry.push_back(&clazz);

    return 0;
}

void jni_resolve_native(so_module *so)
{
    auto &class_registry = ClassRegistry::get_class_registry();
    for (auto clazz: class_registry) {
        if (!clazz->native_methods)
            continue;

        for (int i = 0; clazz->native_methods[i].name != NULL; i++) {
            const NativeMethod &method = clazz->native_methods[i];
            if (strcmp(so->soname, method.soname) == 0) {
                void *sym = (void*)so_symbol(so, method.name);
                if (sym)
                    *method.ptr = sym;
            }
        }
    }
}