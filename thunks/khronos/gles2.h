#pragma once

#include "glad_egl.h"
#include "glad.h"

extern void load_egl_funcs();
extern void load_gles2_funcs();
extern void set_gles2_shader_override_dir(const char* path, bool should_dump);
