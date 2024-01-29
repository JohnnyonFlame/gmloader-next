#if defined(__aarch64__)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "so_util.h"
#include "arm64_encodings.h"

extern uintptr_t so_alloc_arena(so_module *so, uintptr_t range, uintptr_t dst, size_t sz);

void hook_address(so_module *mod, uintptr_t addr, uintptr_t dst) {
  if (addr == 0)
    return;

  // Allocate and populate trampoline
  uint32_t trampoline[4];
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
    trampoline[0] = LDR_LIT_QWORD(X17, 0x8);
    trampoline[1] = BR(R17);
    *(uint64_t *)(trampoline + 2) = dst;
    memcpy((void *)trampoline_addr, (void *)trampoline, sizeof(trampoline));
  } else {
    // Unsafe 4 dword (!!!) backup strategy - clobbers instructions of stubbed
    // functions, and has caused crashes in the past.
    // guest -> host
    trampoline[0] = LDR_LIT_QWORD(X17, 0x8);
    trampoline[1] = BR(R17);
    *(uint64_t *)(trampoline + 2) = dst;
    memcpy((void *)addr, (void *)trampoline, sizeof(trampoline));
  }
}
#endif
