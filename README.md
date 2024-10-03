# GMLoader-next:
A repository for _further_ experimenting with elf loading and in-place patching of android native libraries on non-android operating systems.

This attempts to fix several shortcomings the previous versions of GMLoader had, offering a more compliant compatibility layer, and an elf loader that supports even more relocation types.

### Disclaimers:
-----
This repository contains a lot of incomplete, experimental and untested code, and behavior is not guaranteed to match specifications, specially when it comes to the fake JNI implementation.

Corrections, fixes, issue reports and optimizations are always welcome.


### Building and Deploying [example]:
-----

- `ARCH`: Specify the architecture, e.g.: `aarch64-linux-gnu`
- `LLVM_FILE`: Specify the LLVM Clang library file, e.g.: `/usr/lib/llvm-9/lib/libclang-9.so.1` for clang-9.
- `LLVM_INC`: Specify the path for LLVM includes for your architecture, e.g.: `aarch64-linux-gnu`.
- `OPTM`: Specify the optimization flags, e.g.: `-O3`, `-Os` or `-Og -ggdb`.

```bash
make -f Makefile.gmloader ARCH=aarch64-linux-gnu
```

In order to deploy, you must copy the `lib` redist folder in the application's folder,
those files are part of the runtime, and are required to provide the functionality needed
by the runner.

See [the related documentation](lib/README) for reference.

### Debugging:
-----
The android libraries can be debugged with `gdb` using a breakpoint trick - check out [the provided debugging example](debug.gdb).

For this to be possible, you must extract the libraries from the APK into the application's library folder following the same structure as you would on the APK.

### Config file
-----
`config.json` can be used to configure some parameters. Here is an exemple:

```json
{
    "save_dir" : "gamedata",
    "apk_path" : "my_game.apk",
    "show_cursor": false
}
```

### License:
-----
This is free software. The source files in this repository are released under the [GPLv2 License](LICENSE.md), see the license file for more information.