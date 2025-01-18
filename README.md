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
- `LLVM_INC`: Specify the path for LLVM includes for your architecture, e.g.: `aarch64-linux-gnu`.
- `OPTM`: Specify the optimization flags, e.g.: `-O3`, `-Os` or `-Og -ggdb`.

```bash
make -f Makefile.gmloader ARCH=aarch64-linux-gnu
```

As an example, you can build this using Debian Bullseye to target older platforms:

```bash
make -f Makefile.gmloader \
ARCH=aarch64-linux-gnu \
LLVM_FILE=/usr/lib/llvm-11/lib/libclang-11.so.1 \
LLVM_INC=/usr/aarch64-linux-gnu/include/c++/10/aarch64-linux-gnu \
-j$(nproc)
python3 scripts/generate_libc.py aarch64-linux-gnu --llvm-includes /usr/aarch64-linux-gnu/include/c++/10/aarch64-linux-gnu --llvm-library-file "/usr/lib/llvm-11/lib/libclang-11.so.1"
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

The android libraries can be debugged with `gdb` using a breakpoint trick - check out [the provided debugging example](debug.gdb).

For this to be possible, you must extract the libraries from the APK into the application's library folder following the same structure as you would on the APK.

### Config file
-----
GMLoader-next can load a json formatted configuration file using the `-c` option. For exemple `./gmloadernext.aarch64 -c gmloader.json`

**gmloader.json**:
```json
{
    "save_dir" : "gamedata",
    "apk_path" : "my_game.apk",
    "show_cursor" : false,
    "disable_controller" : false,
    "disable_depth" : false,
    "force_platform" : "os_windows"
}
```

When no configuration file is present the default values are:

| Parameter name     | Default value |
|--------------------|---------------|
| save_dir           | ./            |
| apk_path           | game.apk      |
| show_cursor        | true          |
| disable_controller | false         |
| disable_depth      | false         |
| force_platform     | os_android    |

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
