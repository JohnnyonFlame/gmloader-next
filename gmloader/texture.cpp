#include <string>
#include <filesystem>
#include <math.h>
#include <stdlib.h>
#include "platform.h"
#include "so_util.h"
#include "io_util.h"
#include "libyoyo.h"
#include "configuration.h"

#define STB_ONLY_PNG
#include "stb_image.h"
#include "io_util.h"
#include "thunks/khronos/glad.h"

namespace fs = std::filesystem;

int image_preload_idx = 0;
int setup_ended = 0;

typedef struct pvrtc_file {
    uint32_t Version = 0x03525650;
    uint32_t Flags = 0;
    uint64_t Format = 0;
    uint32_t ColourSpace = 0;
    uint32_t ChannelType = 0;
    uint32_t Height = 0;
    uint32_t Width = 0;
    uint32_t Depth = 1;
    uint32_t NumSurfaces = 1;
    uint32_t NumFaces = 1;
    uint32_t MipCount = 1;
    uint32_t MetadataSize = 0;
    uint8_t Data[];
} __attribute__((packed)) pvrtc_file;

void LoadTextureFromPNG_generic(void *arg1, int arg2, uint32_t *flags, uint32_t *tex_id, uint32_t *texture) {
    int width = 0, height = 0;
    uint32_t *data = ReadPNGFile(arg1, arg2, &width, &height, (*flags & 2) == 0);
    if (data) { 
        InvalidateTextureState();
        glGenTextures(1, tex_id);
        glBindTexture(GL_TEXTURE_2D, *tex_id);

        if (width == 2 && height == 1) {
            if (data[0] == 0xFFBEADDE) {
                int fd;
                pvrtc_file *ext_data = NULL;
                size_t ext_data_sz = 0;
                uint32_t idx = (data[1] << 8) >> 8;

                // Load from APK first and then try the gamedir
                fs::path pvr_path = fs::path(gmloader_config.save_dir)  / "textures" / (std::to_string(idx) + ".pvr");
                int ret = io_load_file(pvr_path.c_str(), (void **)&ext_data, &ext_data_sz);
                if (ret != 1) {
                    fatal_error("Failed to load '%s'!\n", pvr_path.c_str());
                    exit(1);
                }

                if (ext_data) {
                    uintptr_t texture_data = ((uintptr_t)&ext_data->Data) + ext_data->MetadataSize;
                    size_t texture_data_size = ((uintptr_t)ext_data) + ext_data_sz - texture_data;

                    width = ext_data->Width;
                    height = ext_data->Height;
                    switch (ext_data->Format) {
                    case 0x00:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x01:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x02:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x03:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x04:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x05:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x06:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x07:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x09:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x18: /* FUCKS SAKE, THIS IS WRONG, BUT OLDER GMLOADER VERSIONS HAD THE WRONG FORMAT HERE. */
                    case 0x1B:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_4x4_KHR, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x1D:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_5x5_KHR, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    case 0x1F:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_6x6_KHR, width, height, 0, texture_data_size, (void*)texture_data);
                        break;
                    default:
                        warning("Unknown file format %08lX\n", ext_data->Format);   
                    }
                    GLint err = glGetError();
                    if (err != GL_NO_ERROR) {
                        fatal_error("Failed to upload texture, 0x%04X\n", err);
                    }

                    free(ext_data);
                }
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        *flags = *flags | 0x40;
        FreePNGFile();
        texture[0] = 0x06;
        if (flags != &texture[2]) {
            texture[1] = width;
            texture[2] = height;
        } else {
            texture[1] = ((width * *g_TextureScale - 1) | texture[1] & 0xFFFFE000) & 0xFC001FFF | ((height * *g_TextureScale - 1) << 13);
        }
    } else {
        fatal_error("ERROR: Failed to load a PNG texture!\n");
    }
}

typedef struct png_color
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
} png_color;

typedef struct png_info
{
   uint32_t width;
   uint32_t height;
   uint32_t valid;

   size_t rowbytes;
   png_color palette;
   uint16_t num_palette;
   uint16_t num_trans;
   unsigned char bit_depth;
   unsigned char color_type;

   unsigned char compression_type;
   unsigned char filter_type;
   unsigned char interlace_type;
   unsigned char channels;

   unsigned char pixel_depth;
   unsigned char spare_byte;

   unsigned char signature[8];
} png_info;

uint32_t png_get_IHDR_hook(struct png_struct *png_ptr, png_info *info_ptr, uint32_t *width, uint32_t *height, int *bit_depth, int *color_type, int *interlace_type, int *compression_type, int *filter_type)
{
    if (!png_ptr || !info_ptr || !width || !height)
        return 0;
    
    *width = info_ptr->width;
    *height = info_ptr->height;

    if (bit_depth)
        *bit_depth = info_ptr->bit_depth;

    if (color_type)
        *color_type = info_ptr->color_type;

    if (compression_type)
        *compression_type = info_ptr->compression_type;

    if (filter_type)
        *filter_type = info_ptr->filter_type;

    if (interlace_type)
        *interlace_type = info_ptr->interlace_type;

    if (!setup_ended && *width == 2 && *height == 1) {
        fs::path path = fs::path(gmloader_config.save_dir) / "textures" / (std::to_string(image_preload_idx) + ".pvr");
        uint32_t _sz;
        uint32_t *buffer;

        *width = 0;
        *height = 0;

        FILE *f = fopen(path.c_str(), "rb");
        if (f) {
            fseek(f, 0x18, SEEK_SET);
            fread(height, 1, 4, f);
            fread(width, 1, 4, f);
            fclose(f);
        }

        if (*width == 0 || *height == 0) {
            warning("Texture %d metadata preload failure.\n", image_preload_idx);
        }
        
        image_preload_idx++;
    }
    return 1;
}

