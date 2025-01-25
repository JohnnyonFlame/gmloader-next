ZIP_SRC= \
		./3rdparty/libzip/lib//zip_add.c \
		./3rdparty/libzip/lib/zip_add_dir.c \
		./3rdparty/libzip/lib/zip_add_entry.c \
		./3rdparty/libzip/lib/zip_algorithm_deflate.c \
		./3rdparty/libzip/lib/zip_buffer.c \
		./3rdparty/libzip/lib/zip_close.c \
		./3rdparty/libzip/lib/zip_delete.c \
		./3rdparty/libzip/lib/zip_dir_add.c \
		./3rdparty/libzip/lib/zip_dirent.c \
		./3rdparty/libzip/lib/zip_discard.c \
		./3rdparty/libzip/lib/zip_entry.c \
		./3rdparty/libzip/lib/zip_error.c \
		./3rdparty/libzip/lib/zip_error_clear.c \
		./3rdparty/libzip/lib/zip_error_get.c \
		./3rdparty/libzip/lib/zip_error_get_sys_type.c \
		./3rdparty/libzip/lib/zip_error_strerror.c \
		./3rdparty/libzip/lib/zip_error_to_str.c \
		./3rdparty/libzip/lib/zip_extra_field.c \
		./3rdparty/libzip/lib/zip_extra_field_api.c \
		./3rdparty/libzip/lib/zip_fclose.c \
		./3rdparty/libzip/lib/zip_fdopen.c \
		./3rdparty/libzip/lib/zip_file_add.c \
		./3rdparty/libzip/lib/zip_file_error_clear.c \
		./3rdparty/libzip/lib/zip_file_error_get.c \
		./3rdparty/libzip/lib/zip_file_get_comment.c \
		./3rdparty/libzip/lib/zip_file_get_external_attributes.c \
		./3rdparty/libzip/lib/zip_file_get_offset.c \
		./3rdparty/libzip/lib/zip_file_rename.c \
		./3rdparty/libzip/lib/zip_file_replace.c \
		./3rdparty/libzip/lib/zip_file_set_comment.c \
		./3rdparty/libzip/lib/zip_file_set_encryption.c \
		./3rdparty/libzip/lib/zip_file_set_external_attributes.c \
		./3rdparty/libzip/lib/zip_file_set_mtime.c \
		./3rdparty/libzip/lib/zip_file_strerror.c \
		./3rdparty/libzip/lib/zip_fopen.c \
		./3rdparty/libzip/lib/zip_fopen_encrypted.c \
		./3rdparty/libzip/lib/zip_fopen_index_encrypted.c \
		./3rdparty/libzip/lib/zip_fopen_index.c \
		./3rdparty/libzip/lib/zip_fread.c \
		./3rdparty/libzip/lib/zip_fseek.c \
		./3rdparty/libzip/lib/zip_ftell.c \
		./3rdparty/libzip/lib/zip_get_archive_comment.c \
		./3rdparty/libzip/lib/zip_get_archive_flag.c \
		./3rdparty/libzip/lib/zip_get_encryption_implementation.c \
		./3rdparty/libzip/lib/zip_get_file_comment.c \
		./3rdparty/libzip/lib/zip_get_name.c \
		./3rdparty/libzip/lib/zip_get_num_entries.c \
		./3rdparty/libzip/lib/zip_get_num_files.c \
		./3rdparty/libzip/lib/zip_hash.c \
		./3rdparty/libzip/lib/zip_io_util.c \
		./3rdparty/libzip/lib/zip_libzip_version.c \
		./3rdparty/libzip/lib/zip_memdup.c \
		./3rdparty/libzip/lib/zip_name_locate.c \
		./3rdparty/libzip/lib/zip_new.c \
		./3rdparty/libzip/lib/zip_open.c \
		./3rdparty/libzip/lib/zip_pkware.c \
		./3rdparty/libzip/lib/zip_progress.c \
		./3rdparty/libzip/lib/zip_rename.c \
		./3rdparty/libzip/lib/zip_replace.c \
		./3rdparty/libzip/lib/zip_set_archive_comment.c \
		./3rdparty/libzip/lib/zip_set_archive_flag.c \
		./3rdparty/libzip/lib/zip_set_default_password.c \
		./3rdparty/libzip/lib/zip_set_file_comment.c \
		./3rdparty/libzip/lib/zip_set_file_compression.c \
		./3rdparty/libzip/lib/zip_source_accept_empty.c \
		./3rdparty/libzip/lib/zip_set_name.c \
		./3rdparty/libzip/lib/zip_source_begin_write.c \
		./3rdparty/libzip/lib/zip_source_begin_write_cloning.c \
		./3rdparty/libzip/lib/zip_source_buffer.c \
		./3rdparty/libzip/lib/zip_source_call.c \
		./3rdparty/libzip/lib/zip_source_close.c \
		./3rdparty/libzip/lib/zip_source_commit_write.c \
		./3rdparty/libzip/lib/zip_source_compress.c \
		./3rdparty/libzip/lib/zip_source_crc.c \
		./3rdparty/libzip/lib/zip_source_error.c \
		./3rdparty/libzip/lib/zip_source_file_common.c \
		./3rdparty/libzip/lib/zip_source_file_stdio.c \
		./3rdparty/libzip/lib/zip_source_free.c \
		./3rdparty/libzip/lib/zip_source_function.c \
		./3rdparty/libzip/lib/zip_source_get_dostime.c \
		./3rdparty/libzip/lib/zip_source_get_file_attributes.c \
		./3rdparty/libzip/lib/zip_source_is_deleted.c \
		./3rdparty/libzip/lib/zip_source_layered.c \
		./3rdparty/libzip/lib/zip_source_open.c \
		./3rdparty/libzip/lib/zip_source_pass_to_lower_layer.c \
		./3rdparty/libzip/lib/zip_source_pkware_decode.c \
		./3rdparty/libzip/lib/zip_source_pkware_encode.c \
		./3rdparty/libzip/lib/zip_source_read.c \
		./3rdparty/libzip/lib/zip_source_remove.c \
		./3rdparty/libzip/lib/zip_source_rollback_write.c \
		./3rdparty/libzip/lib/zip_source_seek_write.c \
		./3rdparty/libzip/lib/zip_source_seek.c \
		./3rdparty/libzip/lib/zip_source_stat.c \
		./3rdparty/libzip/lib/zip_source_supports.c \
		./3rdparty/libzip/lib/zip_source_tell.c \
		./3rdparty/libzip/lib/zip_source_tell_write.c \
		./3rdparty/libzip/lib/zip_source_window.c \
		./3rdparty/libzip/lib/zip_source_write.c \
		./3rdparty/libzip/lib/zip_source_zip_new.c \
		./3rdparty/libzip/lib/zip_source_zip.c \
		./3rdparty/libzip/lib/zip_stat.c \
		./3rdparty/libzip/lib/zip_stat_index.c \
		./3rdparty/libzip/lib/zip_stat_init.c \
		./3rdparty/libzip/lib/zip_strerror.c \
		./3rdparty/libzip/lib/zip_string.c \
		./3rdparty/libzip/lib/zip_unchange.c \
		./3rdparty/libzip/lib/zip_unchange_all.c \
		./3rdparty/libzip/lib/zip_unchange_data.c \
		./3rdparty/libzip/lib/zip_unchange_archive.c \
		./3rdparty/libzip/lib/zip_utf-8.c \
		./3rdparty/libzip/lib/zip_source_file_stdio_named.c \
		./3rdparty/libzip/lib/zip_random_unix.c

ZIP_OBJ=\
	$(patsubst ./3rdparty/libzip/lib/%.c,./build/${ARCH}/libzip/%.c.o,${ZIP_SRC})

# auto-generated by CMake
ZIP_OBJ += \
	build/${ARCH}/libzip/zip_err_str.gen.c.o

# libzip.h and zipconf.h and the flags are auto-generated by CMake
ZIP_CONFIG=\
	-I./3rdparty/libzip-config \
	-include 3rdparty/libzip-config/libzip.${ARCH}.h

ZIP_FLAGS=\
	$(CFLAGS) \
	-Dzip_EXPORTS \
	-I./3rdparty/libzip/lib \
	-fvisibility=hidden

build/${ARCH}/libzip/%.c.o: 3rdparty/libzip/lib/%.c
	@mkdir -p $(@D)
	$(CC) $(ZIP_CONFIG) $(ZIP_FLAGS) -c $< -o $@

build/${ARCH}/libzip/%.gen.c.o: 3rdparty/libzip-config/%.gen.c
	@mkdir -p $(@D)
	$(CC) $(ZIP_CONFIG) $(ZIP_FLAGS) -c $< -o $@
