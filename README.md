# GMLoader-next:
A repository for _further_ experimenting with elf loading and in-place patching of android native libraries on non-android operating systems.

This attempts to fix several shortcomings the previous versions of GMLoader had, offering a more compliant compatibility layer, and an elf loader that supports even more relocation types.

### Disclaimers:
-----
This repository contains a lot of incomplete, experimental and untested code, and behavior is not guaranteed to match specifications, specially when it comes to the fake JNI implementation.

Corrections, fixes, issue reports and optimizations are always welcome.


### Building and Deploying [example]:
-----

- You can specify the architecture of the loader as such:
```bash
make -f Makefile.gmloader ARCH=aarch64-linux-gnu
```

- Or you can build all supported architectures at once:
```bash
make -f Makefile.gmloader cross-all
```

### Debugging:
-----
The android libraries can be debugged with `gdb` using a breakpoint trick - check out [the provided debugging example](debug.gdb).

For this to be possible, you must extract the libraries from the APK into the application's library folder following the same structure as you would on the APK.

### License:
-----
This is free software. The source files in this repository are released under the [GPLv2 License](LICENSE.md), see the license file for more information.