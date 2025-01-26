
#ifdef __aarch64__
#include <string>
#include <filesystem>
#include <math.h>
#include <stdlib.h>
#include "so_util.h"
#include "platform.h"
#include "libyoyo.h"
#include "configuration.h"
#include "texture.h"

ABI_ATTR void LoadTextureFromPNG_1(uintptr_t texture, int has_mips)
{
    void *arg1 = *(void**)(texture + 0x70);       /* Likely PNG payload address */
    int arg2 = *(int*)(texture + 0x78);           /* Likely PNG payload size */
    uint32_t *arg3 = (uint32_t*)(texture + 0x14); /* Address to Flags */
    uint32_t *arg4 = (uint32_t*)(texture + 0x18); /* Address to OpenGL Texture Id */

    LoadTextureFromPNG_generic(arg1, arg2, arg3, arg4, (uint32_t*)texture);
}

ABI_ATTR void LoadTextureFromPNG_2(uintptr_t texture, int has_mips)
{
    void *arg1 = *(void**)(texture + 0x78);       /* Likely PNG payload address */
    int arg2 = *(int*)(texture + 0x80);           /* Likely PNG payload size */
    uint32_t *arg3 = (uint32_t*)(texture + 0x18); /* Address to Flags */
    uint32_t *arg4 = (uint32_t*)(texture + 0x20); /* Address to OpenGL Texture Id */

    LoadTextureFromPNG_generic(arg1, arg2, arg3, arg4, (uint32_t*)texture);
}

void patch_texture(so_module *mod)
{
	if (gmloader_config.disable_texhack) {
		warning("Texture compression hack NOT ENABLED!!!\n");
		return;
	}

    // Hook LoadTextureFromPNG by searching for it's symbol and then scanning for the load signature
	int hooked = 0;
    uint32_t *LoadTextureFromPNG = (uint32_t *)so_symbol(mod, "_Z18LoadTextureFromPNGP7Texture10eMipEnable");
    for (uint32_t *cursor = LoadTextureFromPNG; (cursor - LoadTextureFromPNG) < 40; cursor++)
    {
        uint32_t inst = *cursor;
        uint32_t masked = inst & 0xFFC0001F;
        uint32_t imm12 = ((inst >> 10) & 0xFFF) << 3;

        // Not LDR x0, [.., ....]
        // https://developer.arm.com/documentation/ddi0602/2024-12/Index-by-Encoding/Loads-and-Stores
        if (masked != 0xF9400000)
            continue;

        switch (imm12)
        {
            case 0x70:
                warning("Using Texture Hack LoadTextureFromPNG_1.\n");
                hook_address(mod, (uintptr_t)LoadTextureFromPNG, (uintptr_t)&LoadTextureFromPNG_1);
                hooked = 1;
                break;
            case 0x78:
                warning("Using Texture Hack LoadTextureFromPNG_2.\n");
                hook_address(mod, (uintptr_t)LoadTextureFromPNG, (uintptr_t)&LoadTextureFromPNG_2);
                hooked = 1;
                break;
            default:
                break;
        }

        if (hooked)
            break;
    }

    if (!hooked) 
    {
        fatal_error(" -- Requested texture_hack, but could not find signature.\n");
        return;
    }

    hook_symbol(mod, "png_get_IHDR", (uintptr_t)&png_get_IHDR_hook, 0);
}

#endif