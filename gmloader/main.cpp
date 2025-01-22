#include <SDL2/SDL.h>
#include <stdio.h>
#include <signal.h>
#include <zip.h>

#include "platform.h"
#include "so_util.h"
#include "io_util.h"
#include "jni.h"
#include "jni_internals.h"
#include "classes/RunnerJNILib.h"
#include "khronos/gles2.h"
#include "libyoyo.h"
#include "configuration.h"

/*
      Don't touch this incantation. It serves no practical
    reason that you can grep this source code for, nothing
    mentions this on the code base, but disturbing what lies
    here will give you headaches, nausea and sleep loss.
      If you *properly* understand why this fixes a stack smash
    on shader loading code, then please, go ahead and explain
    it to me proper, thanks.
*/
thread_local int tls0[2<<12] = {};
int foo() { return tls0[0]++; }

#define CONFIG_FILE     "config.json"

extern DynLibFunction symtable_libc[];
extern DynLibFunction symtable_zlib[];
extern DynLibFunction symtable_gles2[];

extern double FORCE_PLATFORM;

DynLibFunction *so_static_patches[32] = {
    NULL,
};

DynLibFunction *so_dynamic_libraries[32] = {
    symtable_libc,
    symtable_zlib,
    symtable_gles2,
    NULL
};

int load_module(const char *soname, zip_t *apk, so_module &mod, uintptr_t base_address, JavaVM *vm)
{
    void *buffer;
    size_t image_size = 0;
    char filepath[PATH_MAX] = {};
    const char path[] 
#if defined(__aarch64__)
    = "arm64-v8a";
#elif defined(__arm__)
    = "armeabi-v7a";
#else
    = "";
    #error Unknown arch, implement me.
#endif

    snprintf(filepath, PATH_MAX, "lib/%s/%s", path, soname);
    if (io_load_file(filepath, &buffer, &image_size))
        goto load_module_success;

    if (zip_load_file(apk, filepath, &image_size, &buffer, 0))
        goto load_module_success;

    fatal_error("Failed initializing module '%s'.\n", soname);
    return 0;

load_module_success:
    // Now link the module
    int ret = so_load(&mod, filepath, base_address, buffer, image_size) == 0;

    // And call the JNI_OnLoad method if present
    auto JNI_OnLoad = (jint (*)(JavaVM *vm, void *reserved))so_symbol(&mod, "JNI_OnLoad");
    if (JNI_OnLoad != NULL)
        JNI_OnLoad(vm, NULL);

    return ret;
}

so_module libm = {};
so_module libcrt = {};
so_module stdcpp = {};
so_module openal = {};
so_module libyoyo = {};

int RunnerJNILib_MoveTaskToBackCalled = 0;

static fs::path get_absolute_path(const char* path, fs::path work_dir){

    fs::path fs_path = fs::path(path);
    
    if ( fs_path.is_relative() ){
        fs_path = work_dir / fs_path;
    }

    return fs_path;
}

int main(int argc, char *argv[])
{
    gmloader_config.init_defaults();

    fs::path work_dir, config_file_path, save_dir, apk_path;
    work_dir = fs::canonical(fs::current_path()) / "";

    if (argc > 2 && strcmp(argv[1], "-c") == 0) {
        
        config_file_path = work_dir / argv[2];

        if(gmloader_config.parse_file(config_file_path.c_str()) < 0 ){
            warning("Error while loading the config file\n");
        }

    }

    char platform_ov[32];
    strncpy(platform_ov, gmloader_config.force_platform.c_str(), sizeof(platform_ov) - 1);
    platform_ov[sizeof(platform_ov) - 1] = '\0';
    
    std::unordered_map<std::string, int> platform_map = {
        {"os_unknown", os_unknown},
        {"os_windows", os_windows},
        {"os_macosx", os_macosx},
        {"os_ios", os_ios},
        {"os_android", os_android},
        {"os_linux", os_linux},
        {"os_psvita", os_psvita},
        {"os_ps4", os_ps4},
        {"os_xboxone", os_xboxone},
        {"os_tvos", os_tvos},
        {"os_switch", os_switch}
    };

    std::string platform_str(platform_ov);
    std::transform(platform_str.begin(), platform_str.end(), platform_str.begin(), ::tolower);
    auto it = platform_map.find(platform_str);
    if (it != platform_map.end()) {
        FORCE_PLATFORM = it->second;
    } else {
        warning("Unexpected platform '%s'.\n", platform_ov);
        strcpy(platform_ov, "os_unknown");
        FORCE_PLATFORM = os_unknown;
    }

    save_dir = get_absolute_path(gmloader_config.save_dir.c_str(), work_dir) / "";
    apk_path = get_absolute_path(gmloader_config.apk_path.c_str(), work_dir);

    int err;
    zip_t *apk = zip_open(apk_path.c_str(), ZIP_RDONLY, &err);
    if (apk == NULL) {
        zip_error_t zerror;
        zip_error_init_with_code(&zerror, err);
        fatal_error("Failure opening APK '%s': %s\n", apk_path.c_str(), zip_error_strerror(&zerror));
        zip_error_fini(&zerror);
        return -1;
    }

    // Create the Fake JavaVM environment
    JavaVM *vm = NULL;
    JNIEnv *env = NULL;
    if (JNI_CreateJavaVM(&vm, &env, NULL) != JNI_OK) {
        fatal_error("Error initializing JNI Interface!\n");
        return -1;
    }

#ifndef NDEBUG
    uintptr_t addr_libm = 0x10000000;
    uintptr_t addr_libcrt = 0x14000000;
    uintptr_t addr_stdcpp = 0x18000000;
    uintptr_t addr_openal = 0x1C000000;
    uintptr_t addr_yoyo = 0x40000000;
#else
    // Release mode should allocate in any way the system deems suitable
    uintptr_t addr_libm = 0;
    uintptr_t addr_libcrt = 0;
    uintptr_t addr_stdcxx = 0;
    uintptr_t addr_openal = 0;
    uintptr_t addr_yoyo = 0;
#endif

    warning("Loading images...\n");
    // libc.so is implicit (implemented by gmloader)
    // libopenal.so is statically patched via so_static_patches (so we can overload static builds)
    if (!load_module("libm.so", apk, libm, addr_libm, vm)) return -1;
    if (!load_module("libcompiler_rt.so", apk, libcrt, addr_libcrt, vm)) return -1;
    if (!load_module("libc++_shared.so", apk, stdcpp, addr_stdcpp, vm)) return -1;
    int has_al = !load_module("libopenal.so", apk, openal, addr_openal, vm); /* Some APKs have external libopenal.so */
    if (!load_module("libyoyo.so", apk, libyoyo, addr_yoyo, vm)) return -1;

    patch_libyoyo(&libyoyo);
    if(gmloader_config.disable_depth == 1) {
        disable_depth();
    }
    patch_input(&libyoyo);
    patch_gamepad(&libyoyo);
    patch_mouse(&libyoyo);
    patch_fmod(&libyoyo);
    patch_gameframe(&libyoyo);

    int *ms_freq = NULL;
    if (has_al)
        FIND_SYMBOL(&openal, ms_freq, "_ZN17ALCdevice_android7ms_freqE");
    if (ms_freq == NULL)
        FIND_SYMBOL(&libyoyo, ms_freq, "_ZN17ALCdevice_android7ms_freqE");

    if (ms_freq != NULL)
    {
        // Patch the default samplerate to something reasonable
        *ms_freq = 22050;
    }

    String *apk_path_arg = (String *)env->NewStringUTF(apk_path.c_str());
    String *save_dir_arg = (String *)env->NewStringUTF(save_dir.c_str());
    String *pkg_dir_arg = (String *)env->NewStringUTF("com.johnny.loader");
    printf("apk_path %s save_dir %s pkg_dir %s\n", apk_path_arg->str, save_dir_arg->str, pkg_dir_arg->str);

    // Initialize SDL with video, audio, joystick, and controller support
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0) {
        fatal_error("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if(gmloader_config.show_cursor == 0) {
        if (SDL_ShowCursor(SDL_DISABLE) < 0) {
            warning("Cannot disable cursor: %s\n", SDL_GetError());
        } else {
            printf("Cursor disabled\n");
        }
    }

    SDL_Window *sdl_win;
    SDL_GLContext sdl_ctx;

    SDL_DisplayMode mode = {};
    if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
        warning("Unable to query display mode, using defaults.");
        mode.w = 640;
        mode.h = 480;
    }

    sdl_win = SDL_CreateWindow("GMLoader", 0, 0, mode.w, mode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    if (sdl_win == NULL) {
        fatal_error("Failed to create SDL Window: %s\n", SDL_GetError());
        return -1;
    }

    // Basic OpenGL ES 2.x setup
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    sdl_ctx = SDL_GL_CreateContext(sdl_win);
    if (sdl_ctx == NULL) {
        fatal_error("Failed to create OpenGL Context: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_MakeCurrent(sdl_win, sdl_ctx);

    // The libraries are loaded by SDL2.0, and the API entry points by the following
    // functions using the GLAD generated headers.
    load_egl_funcs();
    load_gles2_funcs();

    int cont = 1;
    int w, h;

    RunnerJNILib::Startup(env, 0, apk_path_arg, save_dir_arg, pkg_dir_arg, 4, 0);
    while (cont != 0 && cont != 2 && RunnerJNILib_MoveTaskToBackCalled == 0) {
        if (update_inputs(sdl_win) != 1)
            break;
        SDL_GetWindowSize(sdl_win, &w, &h);
        cont = RunnerJNILib::Process(env, 0, w, h, 0, 0, 0, 0, 0, 60);
        if (RunnerJNILib::canFlip(env, 0))
            SDL_GL_SwapWindow(sdl_win);
    }

    SDL_GL_DeleteContext(sdl_ctx);
    SDL_DestroyWindow(sdl_win);
    SDL_Quit();

    return 0;
}
