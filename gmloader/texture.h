#pragma once
#include <stdint.h>

extern int setup_ended;
extern void LoadTextureFromPNG_generic(void *arg1, int arg2, uint32_t *flags, uint32_t *tex_id, uint32_t *texture);
uint32_t png_get_IHDR_hook(struct png_struct *png_ptr, struct png_info *info_ptr, uint32_t *width, uint32_t *height, int *bit_depth, int *color_type, int *interlace_type, int *compression_type, int *filter_type);
