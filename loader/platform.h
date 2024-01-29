#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdint.h>

// Elf attributes
#if defined(__x86_64) || defined(__aarch64__)
    #define Elf_Addr Elf64_Addr
    #define Elf_Ehdr Elf64_Ehdr
    #define Elf_Shdr Elf64_Shdr
    #define Elf_Phdr Elf64_Phdr
    #define Elf_Dyn Elf64_Dyn
    #define Elf_Sym Elf64_Sym
    #define Elf_Rel Elf64_Rel
    #define Elf_Rela Elf64_Rela
    #define Elf_Relr Elf64_Xword
    #define Elf_Word Elf64_Word
    #define Elf_Sword Elf64_Sxword
    #define ELF_R_TYPE(X)	ELF64_R_TYPE(X)
    #define ELF_R_SYM(X)	ELF64_R_SYM(X)
    #define REL_DYN ".rela.dyn"
    #define REL_PLT ".rela.plt"
#else
    #define Elf_Addr Elf32_Addr
    #define Elf_Ehdr Elf32_Ehdr
    #define Elf_Shdr Elf32_Shdr
    #define Elf_Phdr Elf32_Phdr
    #define Elf_Dyn Elf32_Dyn
    #define Elf_Sym Elf32_Sym
    #define Elf_Rel Elf32_Rel
    #define Elf_Rela Elf32_Rela
    #define Elf_Relr Elf32_Word
    #define Elf_Word Elf32_Word
    #define Elf_Sword Elf32_Sword
    #define ELF_R_TYPE(X)	ELF32_R_TYPE(X)
    #define ELF_R_SYM(X)	ELF32_R_SYM(X)
    #define REL_DYN ".rel.dyn"
    #define REL_PLT ".rel.plt"
#endif


// Definitions for ABI attributes
// Flags bridges and native library functions as the correct ABI, e.g. for
// compiling with HardFP toolchains and drivers, since armv7a android libraries are
// compiled to the SoftFP ABI.
#if __ARM_PCS_VFP==1
#define ABI_ATTR __attribute__((pcs("aapcs")))
#else
#define NO_ABI_ATTR
#define ABI_ATTR
#endif

#define ARCH_ARMV6 (defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__))

#ifdef DEBUG
    #define fatal_error(msg, ...) { fprintf(stderr, "%s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__); }
    #define warning(msg, ...) { fprintf(stderr, msg, ##__VA_ARGS__); }
    #define WARN_STUB fprintf(stderr, "Warning, stubbed function \"%s\".\n", __FUNCTION__);
#else
    #define fatal_error(msg, ...) { fprintf(stderr, "%s:%d: " msg, __FILE__, __LINE__, ##__VA_ARGS__); }
    #define warning(msg, ...) { fprintf(stderr, msg, ##__VA_ARGS__); }
    #define WARN_STUB
#endif

#ifdef __builtin_types_compatible_p
#define __must_be_array(a) \
 BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#else
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MIN_FREE_MEM (48 * 1024 * 1024)

#if __LONG_WIDTH__ == 32
typedef int bionic_off_t; // 32bit off_t type (BIONIC quirk - see https://cs.android.com/android/platform/superproject/+/master:bionic/docs/32-bit-abi.md)
#else
typedef long bionic_off_t;
#endif

#endif /* __PLATFORM_H__ */