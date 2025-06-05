#ifndef __SO_UTIL_H__
#define __SO_UTIL_H__

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>
#include "elf.h"

// RELR/ANDROID_RELA, etc
#define	DT_RELRSZ	35	/* Total size of ElfNN_Relr relocations. */
#define	DT_RELR		36	/* Address of ElfNN_Relr relocations. */
#define	DT_RELRENT	37	/* Size of each ElfNN_Relr relocation. */
#define DT_ANDROID_RELR 0x6fffe000
#define DT_ANDROID_RELRSZ 0x6fffe001
#define DT_ANDROID_RELRENT 0x6fffe003
#define SHT_ANDROID_REL  (SHT_LOOS + 1)
#define SHT_ANDROID_RELA (SHT_LOOS + 2)
#define RELOCATION_GROUPED_BY_INFO_FLAG         1
#define RELOCATION_GROUPED_BY_OFFSET_DELTA_FLAG 2
#define RELOCATION_GROUPED_BY_ADDEND_FLAG       4
#define RELOCATION_GROUP_HAS_ADDEND_FLAG        8

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN_MEM(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ENSURE_SYMBOL(mod, symbol, ...) \
  { \
    const char *aliases[] = {__VA_ARGS__}; \
    for (uint __i = 0; __i < ARRAY_SIZE(aliases); __i++) { \
      if ((*(uintptr_t*)&symbol = (uintptr_t)so_symbol(mod, aliases[__i]))) \
        break; \
    } \
    if (symbol == NULL) { \
      fatal_error("Symbol \"%s\" not found.\n", #symbol); \
      exit(-1); \
    } \
  }

#define FIND_SYMBOL(mod, symbol, ...) \
  { \
    const char *aliases[] = {__VA_ARGS__}; \
    for (uint __i = 0; __i < ARRAY_SIZE(aliases); __i++) { \
      if ((*(uintptr_t*)&symbol = (uintptr_t)so_symbol(mod, aliases[__i]))) \
        break; \
    } \
    if (symbol == NULL) { \
      warning("Symbol \"%s\" not found.\n", #symbol); \
    } \
  }

#define MAX_DATA_SEG 16
ABI_ATTR typedef int (* init_array_t)();
typedef struct so_module {
  struct so_module *next;

  // The cave and patch arenas are both usable for code generation (e.g. for code instrumentation).
  // patch arena is allocated prior to the .text segment, while the cave is the padding region used
  // to align segments, and thus left free to use as a code cave (see "p_align" member of the
  // program header table entries).
  uintptr_t patch_base, patch_head, cave_base, cave_head, text_base, data_base[MAX_DATA_SEG];
  size_t patch_size, cave_size, text_size, data_size[MAX_DATA_SEG];
  int n_data;

  Elf_Addr base;

  Elf_Ehdr *ehdr;
  Elf_Phdr *phdr;
  Elf_Shdr *shdr;

  Elf_Dyn *dynamic;
  Elf_Sym *dynsym;
  Elf_Rel *reldyn;
  Elf_Rel *relplt;
  uint8_t *droidreladyn; //packed sleb128 entries
  uint8_t *droidreldyn;
  Elf_Rela *reladyn;
  Elf_Rela *relaplt;
  void *relr; //rle bitmask entries

  init_array_t *init_array;
  uint32_t *hash;

  int num_dynamic;
  int num_dynsym;
  int num_reldyn;
  int num_relplt;
  int num_droidreladyn;
  int num_droidreldyn;
  int num_reladyn;
  int num_relaplt;
  int num_init_array;
  int num_relr;

  char *soname;
  char *shstr;
  char *dynstr;
} so_module;

typedef struct {
  const char *symbol;
  uintptr_t func;
} DynLibFunction;

typedef struct {
  char *symbol;
  uintptr_t hook;
  int opt;
} DynLibHooks;

/* 
  Do you need a hook that does something and then goes back to doing the normal
  code path? Use this.
*/
typedef struct {
  uintptr_t addr;
  uint32_t prologue[4]; /* worst case scenario */
  uint32_t trampoline[4]; /* in case we want to re-hook it */
} ReentrantHook;

void hook_address(so_module *mod, uintptr_t addr, uintptr_t dst);
void hook_symbol(so_module *mod, const char *symbol, uintptr_t dst, int is_optional);
void hook_symbols(so_module *mod, DynLibHooks *hooks);
void rehook_new(so_module *mod, ReentrantHook *hook, uintptr_t addr, uintptr_t dst);
void rehook_hook(ReentrantHook *hook);
void rehook_unhook(ReentrantHook *hook);

int so_load(so_module *mod, const char *filename, uintptr_t load_addr, void *so_data, size_t sz);
void so_relocate(so_module *mod);
int so_static_overrides(so_module *mod);
uintptr_t so_resolve_link(so_module *mod, const char *symbol);
void so_initialize(so_module *mod);
uintptr_t so_symbol(so_module *mod, const char *symbol);
int so_symbol_index(so_module *mod, const char *symbol);
void so_symbol_fix_ldmia(so_module *mod, const char *symbol);
uint32_t so_hash(const uint8_t *name);

// Defined on a per-port basis on their specific main.c files
extern DynLibFunction *so_static_patches[];    // Functions to be replaced in the binary
extern DynLibFunction *so_dynamic_libraries[]; // Functions to be resolved

#ifdef __cplusplus
};
#endif

#endif
