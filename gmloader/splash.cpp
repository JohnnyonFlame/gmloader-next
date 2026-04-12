#include <filesystem>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "io_util.h"
#include "khronos/gles2.h"
#include "configuration.h"
#include "splash.h"

#define STB_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "stb_image.h"

namespace fs = std::filesystem;

static const char *splash_vert_src =
    "#version 100\n"
    "varying vec2 vTexCoord;\n"
    "attribute vec2 aVertCoord;\n"
    "attribute vec2 aTexCoord;\n"
    "void main() {\n"
    "   vTexCoord = aTexCoord;\n"
    "   gl_Position = vec4(aVertCoord, 0.0, 1.0);\n"
    "}";

static const char *splash_frag_src =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 vTexCoord;\n"
    "uniform sampler2D uTex;\n"
    "void main() {\n"
    "   gl_FragColor = texture2D(uTex, vTexCoord);\n"
    "}\n";

static bool load_splash_png(zip_t *apk, uint8_t **pixels, int *img_w, int *img_h)
{
    void *buf = NULL;
    size_t buf_sz = 0;
    int _;

    fs::path on_disk = fs::path(gmloader_config.save_dir) / "splash.png";
    if (!io_load_file(on_disk.c_str(), &buf, &buf_sz)) {
        if (!zip_load_file(apk, "assets/splash.png", &buf_sz, &buf, 0)) {
            buf = NULL;
        }
    }

    if (!buf)
        return false;

    *pixels = stbi_load_from_memory((stbi_uc *)buf, buf_sz, img_w, img_h, &_, 4);
    free(buf);
    return *pixels != NULL;
}

static GLuint create_splash_texture(zip_t *apk, int *img_w, int *img_h, int *tex_w, int *tex_h)
{
    uint8_t *pixels = NULL;

    if (!load_splash_png(apk, &pixels, img_w, img_h)) {
        warning("Warning, no splash image available, creating xor pattern...\n");
        *img_w = 320;
        *img_h = 240;
        pixels = (uint8_t *)malloc(320 * 240 * 4);
        uint32_t *p = (uint32_t *)pixels;
        for (int i = 0; i < 240; i++) {
            for (int j = 0; j < 320; j++) {
                p[i * 320 + j] = ((~i ^ j) << 16) | ((i ^ ~j) << 8) | (i ^ j);
            }
        }
    }

    *tex_w = 1; while (*tex_w < *img_w) *tex_w *= 2;
    *tex_h = 1; while (*tex_h < *img_h) *tex_h *= 2;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *tex_w, *tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, *img_w, *img_h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glFlush();

    free(pixels);
    return tex;
}

static void compute_aspect_quad(int vp_w, int vp_h, int img_w, int img_h,
                                float u, float v, float verts[16])
{
    float aspect_img = (float)img_w / (float)img_h;
    float aspect_vp = (float)vp_w / (float)vp_h;
    float rx, ry;

    if (aspect_vp > aspect_img) {
        rx = aspect_img / aspect_vp;
        ry = 1.0f;
    } else {
        rx = 1.0f;
        ry = aspect_vp / aspect_img;
    }

    float sx = (1.0f - rx) / 2.0f;
    float sy = (1.0f - ry) / 2.0f;

    // tri-strip: BL, TL, BR, TR  (with V flipped since image origin is top-left)
    verts[0]  = -1.0f + sx; verts[1]  = -1.0f + sy; verts[2]  = 0.0f; verts[3]  = v;
    verts[4]  = -1.0f + sx; verts[5]  =  1.0f - sy; verts[6]  = 0.0f; verts[7]  = 0.0f;
    verts[8]  =  1.0f - sx; verts[9]  = -1.0f + sy; verts[10] = u;    verts[11] = v;
    verts[12] =  1.0f - sx; verts[13] =  1.0f - sy; verts[14] = u;    verts[15] = 0.0f;
}

void splash_render(zip_t *apk, int vp_w, int vp_h)
{
    int img_w = 0, img_h = 0, tex_w = 0, tex_h = 0;
    GLuint tex = create_splash_texture(apk, &img_w, &img_h, &tex_w, &tex_h);

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &splash_vert_src, NULL);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &splash_frag_src, NULL);
    glCompileShader(frag);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint loc_vert = glGetAttribLocation(prog, "aVertCoord");
    GLint loc_uv   = glGetAttribLocation(prog, "aTexCoord");
    GLint loc_tex  = glGetUniformLocation(prog, "uTex");

    float verts[16];
    float u = (float)img_w / (float)tex_w;
    float v = (float)img_h / (float)tex_h;
    compute_aspect_quad(vp_w, vp_h, img_w, img_h, u, v, verts);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glViewport(0, 0, vp_w, vp_h);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(prog);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(loc_tex, 0);

    glEnableVertexAttribArray(loc_vert);
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_vert, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(0));
    glVertexAttribPointer(loc_uv,   2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFinish();

    glDisableVertexAttribArray(loc_vert);
    glDisableVertexAttribArray(loc_uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteBuffers(1, &vbo);
    glDeleteProgram(prog);
    glDeleteShader(vert);
    glDeleteShader(frag);
    glDeleteTextures(1, &tex);
}
