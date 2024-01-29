#include <stdio.h>
#include <SDL2/SDL.h>
#include <zip.h>
#include "platform.h"
#include "io_util.h"

int zip_load_file(struct zip *zip, const char *file, size_t *inflated_bytes, void **bytes, size_t max_bytes)
{
    /* Attempt to locate the file */
    /* Get the uncompressed size */
    zip_stat_t so_stat;
    if (zip_stat(zip, file, 0, &so_stat) == -1) {
        fatal_error("Failed to stat file '%s'.\n", file);
        return 0;
    }

    zip_file_t *f = zip_fopen_index(zip, so_stat.index, ZIP_FL_UNCHANGED);
    if (f == NULL) {
        fatal_error("Unable to open '%s'!\n", file);
        return 0;
    }

    if (max_bytes == 0) {
        max_bytes = so_stat.size;
    }

    zip_int64_t br;
    void *buf = malloc(max_bytes);
    if ((br = zip_fread(f, buf, max_bytes)) != max_bytes) {
        fatal_error("Reading file failed, read %lld out of %lld bytes.\n", br, so_stat.size);
        zip_fclose(f);
        free(buf);
        return 0;
    }

    *inflated_bytes = max_bytes;
    *bytes = buf;

    zip_fclose(f);
    return 1;
}

extern int io_load_file(const char *filename, void **buf, size_t *size)
{
    SDL_RWops *io = SDL_RWFromFile(filename, "rb");
    if (!io)
        return 0;

    Sint64 sz = SDL_RWsize(io);
    if (sz <= 0)
        goto io_load_file_rw;

    *buf = malloc(sz);
    *size = sz;

    if (!buf)
        goto io_load_file_rw;

    if (SDL_RWread(io, *buf, sz, 1) < 1)
        goto io_load_file_buf;

    return 1;

io_load_file_buf:
    free(*buf);
io_load_file_rw:
    SDL_RWclose(io);
    return 0;
}