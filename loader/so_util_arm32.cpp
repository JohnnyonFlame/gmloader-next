#if defined(__arm__)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "so_util.h"
#include "arm32_encodings.h"

extern uintptr_t so_alloc_arena(so_module *so, uintptr_t range, uintptr_t dst, size_t sz);

static void trampoline_ldm(so_module *mod, uint32_t *dst)
{
  uint32_t trampoline[1];
  uint32_t funct[20] = {0xFAFAFAFA};
  uint32_t *ptr = funct;

  int cur = 0;
  int baseReg = ((*dst) >> 16) & 0xF;
  int bitMask =  (*dst)        & 0xFFFF;

  uint32_t stored = NULL;
  for (int i = 0; i < 16; i++) {
    if (bitMask & (1 << i)) {
      // If the register we're reading the offset from is the same as the one we're writing,
      // delay it to the very end so that the base pointer ins't clobbered
      if (baseReg == i)
        stored = LDR_OFFS(i, baseReg, cur);
      else
        *ptr++ = LDR_OFFS(i, baseReg, cur);
      cur += 4;
    }
  }

  // Perform the delayed load if needed
  if (stored) {
    *ptr++ = stored;
  }

  *ptr++ = LDR_OFFS(PC, PC, -0x4); // LDR PC, [PC, -0x4]  ; jmp to [dst+0x4]
  *ptr++ = (uint32_t)(dst+1);      // .dword <...>        ; [dst+0x4]

  size_t trampoline_sz =  ((uintptr_t)ptr - (uintptr_t)&funct[0]);
  uintptr_t patch_addr = so_alloc_arena(mod, B_RANGE, (uintptr_t)B_OFFSET(dst), trampoline_sz);

  if (!patch_addr) {
    fatal_error("Failed to patch ldmia at 0x%08X, unable to allocate space.\n", dst);
    exit(-1);
  }
  
  // Create sign extended relative address rel_addr
  trampoline[0] = B(dst, patch_addr);

  memcpy((void*)patch_addr, funct, trampoline_sz);
  memcpy(dst, trampoline, sizeof(trampoline));
}

void so_symbol_fix_ldmia(so_module *mod, const char *symbol)
{
  // This is meant to work around crashes due to unaligned accesses (SIGBUS :/) due to certain
  // kernels not having the fault trap enabled, e.g. certain RK3326 Odroid Go Advance clone distros.
  // TODO:: Maybe enable this only with a config flag? maybe with a list of known broken functions?
  // Known to trigger on GM:S's "_Z11Shader_LoadPhjS_" - if it starts happening on other places,
  // might be worth enabling it globally.
  
  int idx = so_symbol_index(mod, symbol);
  if (idx == -1)
    return;

  uintptr_t st_addr = mod->base + mod->dynsym[idx].st_value;
  for (uintptr_t addr = st_addr; addr < st_addr + mod->dynsym[idx].st_size; addr+=4) {
    uint32_t inst = *(uint32_t*)(addr);
    
    //Is this an LDMIA instruction with a R0-R12 base register?
    if (((inst & 0xFFF00000) == 0xE8900000) && (((inst >> 16) & 0xF) < SP) ) {
      warning("Found possibly misaligned ldmia on %p, trying to fix it... (instr: %p, to %p)\n",
        (void*)addr, *(void**)addr, (void*)mod->patch_head);
      trampoline_ldm(mod, (uint32_t*)addr);
    }
  }
}

void hook_thumb(so_module *mod, uintptr_t addr, uintptr_t dst) {
  if (addr == 0)
    return;
  addr &= ~1;

  // Align to word boundary with no-op
  if (addr & 2) {
    uint16_t nop = 0x46c0; // NO-OP (MOV R8, R8)
    memcpy((void *)addr, &nop, sizeof(nop));
    addr += 2;
  }

  uint32_t hook[2];
#if ARCH_ARMV6
  #error deprecated
  // the BRANCH instruction is offset by a word due to the
  // thumb-to-arm transition (aka "BX PC").
  uintptr_t b_addr = addr + 4;

  // populate the (addr -> trampoline -> dst) trampoline.
  uint32_t trampoline[2];
  uintptr_t trampoline_addr = so_alloc_arena(mod, B_RANGE, B_OFFSET(b_addr), 2 * sizeof(uint32_t));
  trampoline[0] = LDR_OFFS(PC, PC, -4);
  trampoline[1] = dst;
  memcpy(trampoline_addr, trampoline, sizeof(trampoline));

  // Leave Thumb-mode and branch into trampoline
  hook[0] = 0x46c04778; // BX PC ; NO-OP (MOV R8, R8)
  hook[1] = B(b_addr, trampoline_addr);
#else
  // Jump straight into the routine - no need for trampolines on ARMv7
  hook[0] = 0xf000f8df; // LDR PC, [PC]
  hook[1] = dst;
#endif
  memcpy((void *)addr, hook, sizeof(hook));
}

void hook_arm(so_module *mod, uintptr_t addr, uintptr_t dst) {
  if (addr == 0)
    return;

  // Allocate and populate trampoline
  uint32_t trampoline[2];
  uintptr_t trampoline_addr = so_alloc_arena(mod, B_RANGE, B_OFFSET(addr), sizeof(trampoline));
  if (trampoline_addr) {
    // Safer 1 dword, two step trampoline strategy
    // guest -> patch arena -> host

    // step 1:
    // guest -> patch arena <...> 
    uint32_t hook[1];
    hook[0] = B(addr, trampoline_addr);
    memcpy((void *)addr, hook, sizeof(hook));

    // step 2:
    // patch arena -> host
    trampoline[0] = LDR_OFFS(PC, PC, -0x4); // LDR PC, [PC, #-0x4]
    trampoline[1] = dst;                    // .dw dst
    memcpy((void *)trampoline_addr, trampoline, sizeof(trampoline));
  }
  else {
    // Unsafe 2 dword backup strategy - clobbers instructions of stubbed
    // functions, and has caused crashes in the past.
    // guest -> host
    uint32_t hook[2];
    hook[0] = LDR_OFFS(PC, PC, -0x4); // LDR PC, [PC, #-0x4]
    hook[1] = dst;                    // .dw dst
    memcpy((void *)addr, hook, sizeof(hook));
  }
}

void hook_address(so_module *mod, uintptr_t addr, uintptr_t dst) {
  // Instead of guessing - check LSB for the "thumb" mode bit
  if ((long)addr & 1)
    hook_thumb(mod, addr, dst);
  else
    hook_arm(mod, addr, dst);
}
#endif
