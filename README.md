# GMLoader-next:
A repository for _further_ experimenting with elf loading and in-place patching of android native libraries on non-android operating systems.

This attempts to fix several shortcomings the [previous versions of GMLoader](https://github.com/JohnnyonFlame/droidports) had, offering a more compliant compatibility layer, and an elf loader that supports even more relocation types.

### Disclaimers:
-----
This repository contains a lot of incomplete, experimental and untested code, and behavior is not guaranteed to match specifications, specially when it comes to the fake JNI implementation.

Corrections, fixes, issue reports and optimizations are always welcome.


### Building and Deploying [example]:
-----

First clone the repository and all of it's submodules:

`git clone <repository url> --recursive`

Now build the project for your desired target platforms, the following build options are available:

- `ARCH`: Specify the architecture, e.g.: `aarch64-linux-gnu` or `arm-linux-gnueabihf`
- `LLVM_FILE`: Specify the LLVM Clang library file, e.g.: `/usr/lib/llvm-9/lib/libclang-9.so.1` for clang-9.
- `LLVM_SYSROOT`: Specify the path for the toolchain's sysroot, e.g. `${TOOLCHAIN}/arm-linux-gnueabihf`.
- `LLVM_INC`: Specify the path for LLVM includes for your architecture, e.g.: `aarch64-linux-gnu`.
- `OPTM`: Specify the optimization flags, e.g.: `-O3`, `-Os` or `-Og -ggdb`.
- `STATIC_LIBSTDCXX`: Link libstdc++ statically (useful for legacy platforms).
- `USE_FMOD`: Whether to build FMOD Extension support.
    - You need to extract fmodstudioapi.tar.gz into `3rdparty/fmod` for FMOD support.

```bash
make -f Makefile.gmloader ARCH=aarch64-linux-gnu
```

As an example, you can build this using Debian Bullseye to target older platforms:

```bash
# You can manually process the thunks if needed...
python3 scripts/generate_libc.py aarch64-linux-gnu --llvm-includes /usr/aarch64-linux-gnu/include/c++/10/aarch64-linux-gnu --llvm-library-file "/usr/lib/llvm-11/lib/libclang-11.so.1"

# But specifying the LLVM flags to the Makefile works too.
make -f Makefile.gmloader \
ARCH=aarch64-linux-gnu \
LLVM_FILE=/usr/lib/llvm-11/lib/libclang-11.so.1 \
LLVM_INC=/usr/aarch64-linux-gnu/include/c++/10/aarch64-linux-gnu \
-j$(nproc)
```

Or even using custom toolchains, such as [EverSDK](github.com/johnnyonflame/EverSDK):

```bash
export TOOLCHAIN=${HOME}/eversdk/toolchain
export PATH=$PATH:${TOOLCHAIN}/bin
make -f Makefile.gmloader ARCH=arm-linux-gnueabihf \
LLVM_INC="${TOOLCHAIN}/arm-linux-gnueabihf/sysroot/usr/include ${TOOLCHAIN}/arm-linux-gnueabihf/include/c++/13.2.0 ${TOOLCHAIN}/arm-linux-gnueabihf/include/c++/13.2.0/arm-linux-gnueabihf" \
LLVM_SYSROOT="${TOOLCHAIN}/arm-linux-gnueabihf"
```

Now, to deploy, you must copy the `lib` redist folder in the application's folder. Those files are dependencies of the guest Android application (e.g. the runner) and are used to provide
a more accurate Android environment for the guest application.

See [the related documentation](lib/README) for reference.

### Debugging:
-----
Git info is baked into the binary and can be pulled with `strings gmloadernext.${DEVICE_ARCH} | grep -E (GIT_|BUILD_)`, for example:
```
root@SD865: strings gmloadernext.${DEVICE_ARCH} | grep -E (GIT_|BUILD_)
BUILD_DATE_20250103_hh:mm:ss
GIT_BRANCH_master
GIT_HASH_a4b6187a6757cd8cc5eb4af958d2044e2543677b
```

You can toggle VM Tracing on/off with the GMLOADER_TRACE_VM variable, allowing you to see what's being executed by the GameMaker Runner's VM in
real time.

The android libraries can be debugged with `gdb` using a breakpoint trick - check out [the provided debugging example](debug.gdb).

For this to be possible, you must extract the libraries from the APK into the application's library folder following the same structure as you would on the APK.

If you need to investigate deeper, you'll want to enable `#define WANTS_TRACE` in both [Thunk Gen](thunks/thunk_gen.h) and [JNI Implementation](jni/jni.cpp).

### Overriding Shaders:
-----
Some lower end platforms might have either performance issues or outright fail to compile more complex shaders like Scribble ones - when this happens, users
can dump the shaders by setting the `shader_dir` folder, and then enabling the `GMLOADER_DUMP_SHADERS` environment variable.

Upon editing of the problematic shaders, users can remove the redundant ones and unset `GMLOADER_DUMP_SHADERS`, this will cause the game to attempt to override
loaded shaders with equivalent ones from the `shader_dir` path, hopefully resolving whatever problem you were facing.

### Config file
-----
GMLoader-next can load a json formatted configuration file using the `-c` option. For exemple `./gmloadernext.aarch64 -c gmloader.json`

**gmloader.json**:
```json
{
    "save_dir" : "saves",
    "apk_path" : "mygame.apk",
    "shader_dir": "./shaders/",
    "show_cursor" : false,
    "disable_controller" : false,
    "disable_depth" : false,
    "disable_extensions" : false,
    "disable_joystick" : true,
    "disable_rumble" : false,
    "rumble_scale" : 1.0,
    "force_platform" : "os_android"
}
```

When no configuration file is present the default values are as above.

Supported values for force_platform are:
- os_unknown
- os_windows
- os_macosx
- os_ios
- os_android
- os_linux
- os_psvita
- os_ps4
- os_xboxone
- os_tvos
- os_switch

### License:
-----
This is free software. The source files in this repository are released under the [GPLv2 License](LICENSE.md), see the license file for more information.
