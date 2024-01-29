#pragma once

int zip_load_file(struct zip *zip, const char *file, size_t *inflated_bytes, void **bytes, size_t max_bytes);
extern int io_load_file(const char *filename, void **buf, size_t *size);