
#ifdef __arm__
#include <string>
#include <filesystem>
#include <math.h>
#include <stdlib.h>
#include "so_util.h"
#include "platform.h"
#include "libyoyo.h"
#include "configuration.h"
#include "texture.h"

ABI_ATTR void LoadTextureFromPNG_1(uint32_t *texture, int has_mips) {
    LoadTextureFromPNG_generic((void*)texture[23], texture[24], &texture[5], &texture[6], texture);
}

ABI_ATTR void LoadTextureFromPNG_2(uint32_t *texture, int has_mips) {
    LoadTextureFromPNG_generic((void*)texture[11], texture[12], &texture[4], &texture[5], texture);
}

ABI_ATTR void LoadTextureFromPNG_3(uint32_t *texture) {
    LoadTextureFromPNG_generic((void*)texture[9], texture[10], &texture[2], &texture[3], texture);
}

ABI_ATTR void LoadTextureFromPNG_4(uint32_t *texture) {
    LoadTextureFromPNG_generic((void*)texture[8], texture[9], &texture[2], &texture[3], texture);
}

void patch_texture(so_module *mod)
{
	if (gmloader_config.disable_texhack) {
		warning("Texture compression hack NOT ENABLED!!!\n");
		return;
	}

    uint8_t has_mips = 1;
	uint32_t *LoadTextureFromPNG = (uint32_t *)so_symbol(mod, "_Z18LoadTextureFromPNGP7Texture10eMipEnable");
	if (!LoadTextureFromPNG) {
		LoadTextureFromPNG = (uint32_t *)so_symbol(mod, "_Z18LoadTextureFromPNGP7Texture");
		has_mips = 0;
	}

    if (!has_mips) {
		uint32_t *p = LoadTextureFromPNG;
		int found = 0;
		for (;;) {
			if (*p == 0xE5900020) { // LDR R0, [R0,#0x20]
				warning("Patching LoadTextureFromPNG to variant #4\n");
				hook_address(mod, (uintptr_t)&LoadTextureFromPNG, (uintptr_t)&LoadTextureFromPNG_4);
				found = 1;
				break;
			} else if (*p == 0xE5900024) { // LDR R0, [R0,#0x24]
				warning("Patching LoadTextureFromPNG to variant #3\n");
				hook_address(mod, (uintptr_t)&LoadTextureFromPNG, (uintptr_t)&LoadTextureFromPNG_3);
				found = 1;
				break;
			}
			p++;
		}

		if (!found) {
			warning(" -- Requested texture_hack, but could not signature.\n");
			return;
		}
	} else {
		switch (*LoadTextureFromPNG >> 16) {
		case 0xE92D:
			warning("Patching LoadTextureFromPNG to variant #1\n");
			hook_address(mod, (uintptr_t)&LoadTextureFromPNG, (uintptr_t)&LoadTextureFromPNG_1);
			break;
		case 0xE590:
			warning("Patching LoadTextureFromPNG to variant #2\n");
			hook_address(mod, (uintptr_t)&LoadTextureFromPNG, (uintptr_t)&LoadTextureFromPNG_2);
			break;
		default:
			fatal_error("Error: Unrecognized LoadTextureFromPNG signature: 0x%08X.", *LoadTextureFromPNG);
			fatal_error(" -- Requested texture_hack, but could not find signature.\n");
			return;
		}
	}

	hook_symbol(mod, "png_get_IHDR", (uintptr_t)&png_get_IHDR_hook, 0);
}
#endif