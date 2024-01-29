/*
 * Copyright (C) 2012 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <poll.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <unistd.h>
#include <bsd/string.h>

#include <signal.h>
#include "platform.h"

typedef sigset_t sigset64_t;


#define __check_fd_set(...)
#define __check_buffer_access(...)
#define	__predict_true(exp) ((exp) == 0)
#define	__predict_false(exp) ((exp) != 0)
#define __size_mul_overflow(...) (0)
#define __check_count(...)
#define __check_pollfd_array(...)
#define __fortify_fatal(...)

void *ABI_ATTR __memset_chk_impl(void* s, int c, size_t n, size_t n2)
{
    return memset(s, c, n);
}

int ABI_ATTR __FD_ISSET_chk_impl(int fd, const fd_set* set, size_t set_size) {
  __check_fd_set("FD_ISSET", fd, set_size);
  return __FD_ISSET(fd, set);
}

void ABI_ATTR __FD_CLR_chk_impl(int fd, fd_set* set, size_t set_size) {
  __check_fd_set("FD_CLR", fd, set_size);
  __FD_CLR(fd, set);
}

void ABI_ATTR __FD_SET_chk_impl(int fd, fd_set* set, size_t set_size) {
  __check_fd_set("FD_SET", fd, set_size);
  __FD_SET(fd, set);
}

char* ABI_ATTR __fgets_chk_impl(char* dst, int supplied_size, FILE* stream, size_t dst_len_from_compiler) {
  __check_buffer_access("fgets", "write into", supplied_size, dst_len_from_compiler);
  return fgets(dst, supplied_size, stream);
}

size_t ABI_ATTR __fread_chk_impl(void* buf, size_t size, size_t count, FILE* stream, size_t buf_size) {
  size_t total;
  if (__predict_false(__size_mul_overflow(size, count, &total))) {
    // overflow: trigger the error path in fread
    return fread(buf, size, count, stream);
  }
  __check_buffer_access("fread", "write into", total, buf_size);
  return fread(buf, size, count, stream);
}

size_t ABI_ATTR __fwrite_chk_impl(const void* buf, size_t size, size_t count, FILE* stream, size_t buf_size) {
  size_t total;
  if (__predict_false(__size_mul_overflow(size, count, &total))) {
    // overflow: trigger the error path in fwrite
    return fwrite(buf, size, count, stream);
  }
  __check_buffer_access("fwrite", "read from", total, buf_size);
  return fwrite(buf, size, count, stream);
}

extern char* ABI_ATTR __getcwd_chk_impl(char* buf, size_t len, size_t actual_size) {
  __check_buffer_access("getcwd", "write into", len, actual_size);
  return getcwd(buf, len);
}

void* ABI_ATTR __memchr_chk_impl(const void* s, int c, size_t n, size_t actual_size) {
  __check_buffer_access("memchr", "read from", n, actual_size);
  return const_cast<void*>(memchr(s, c, n));
}

// Runtime implementation of __builtin____memmove_chk (used directly by compiler, not in headers).
extern "C" void* ABI_ATTR __memmove_chk_impl(void* dst, const void* src, size_t len, size_t dst_len) {
  __check_buffer_access("memmove", "write into", len, dst_len);
  return memmove(dst, src, len);
}

// memcpy is performance-critical enough that we have assembler __memcpy_chk implementations.
// This function is used to give better diagnostics than we can easily do from assembler.
extern "C" void* ABI_ATTR __memcpy_chk_fail_impl(void* /*dst*/, const void* /*src*/, size_t count, size_t dst_len) {
  __check_count("memcpy", "count", count);
  __check_buffer_access("memcpy", "write into", count, dst_len);
  abort(); // One of the above is supposed to have failed, otherwise we shouldn't have been called.
}

void* ABI_ATTR __memrchr_chk_impl(const void* s, int c, size_t n, size_t actual_size) {
  __check_buffer_access("memrchr", "read from", n, actual_size);
  return memrchr(const_cast<void *>(s), c, n);
}

// memset is performance-critical enough that we have assembler __memset_chk implementations.
// This function is used to give better diagnostics than we can easily do from assembler.
extern "C" void* ABI_ATTR __memset_chk_fail_impl(void* /*dst*/, int /*byte*/, size_t count, size_t dst_len) {
  __check_count("memset", "count", count);
  __check_buffer_access("memset", "write into", count, dst_len);
  abort(); // One of the above is supposed to have failed, otherwise we shouldn't have been called.
}

int ABI_ATTR __poll_chk_impl(pollfd* fds, nfds_t fd_count, int timeout, size_t fds_size) {
  __check_pollfd_array("poll", fds_size, fd_count);
  return poll(fds, fd_count, timeout);
}

int ABI_ATTR __ppoll_chk_impl(pollfd* fds, nfds_t fd_count, const timespec* timeout,
                const sigset_t* mask, size_t fds_size) {
  __check_pollfd_array("ppoll", fds_size, fd_count);
  return ppoll(fds, fd_count, timeout, mask);
}

int ABI_ATTR __ppoll64_chk_impl(pollfd* fds, nfds_t fd_count, const timespec* timeout,
                  const sigset_t* mask, size_t fds_size) {
  __check_pollfd_array("ppoll64", fds_size, fd_count);
  return ppoll(fds, fd_count, timeout, mask); //TODO:: is this right?
}

ssize_t ABI_ATTR __pread64_chk_impl(int fd, void* buf, size_t count, off64_t offset, size_t buf_size) {
  __check_count("pread64", "count", count);
  __check_buffer_access("pread64", "write into", count, buf_size);
  return pread64(fd, buf, count, offset);
}

ssize_t ABI_ATTR __pread_chk_impl(int fd, void* buf, size_t count, off_t offset, size_t buf_size) {
  __check_count("pread", "count", count);
  __check_buffer_access("pread", "write into", count, buf_size);
  return pread(fd, buf, count, offset);
}

ssize_t ABI_ATTR __pwrite64_chk_impl(int fd, const void* buf, size_t count, off64_t offset,
                                  size_t buf_size) {
  __check_count("pwrite64", "count", count);
  __check_buffer_access("pwrite64", "read from", count, buf_size);
  return pwrite64(fd, buf, count, offset);
}

ssize_t ABI_ATTR __pwrite_chk_impl(int fd, const void* buf, size_t count, off_t offset,
                                size_t buf_size) {
  __check_count("pwrite", "count", count);
  __check_buffer_access("pwrite", "read from", count, buf_size);
  return pwrite(fd, buf, count, offset);
}

ssize_t ABI_ATTR __read_chk_impl(int fd, void* buf, size_t count, size_t buf_size) {
  __check_count("read", "count", count);
  __check_buffer_access("read", "write into", count, buf_size);
  return read(fd, buf, count);
}

ssize_t ABI_ATTR __readlinkat_chk_impl(int dirfd, const char* path, char* buf, size_t size, size_t buf_size) {
  __check_count("readlinkat", "size", size);
  __check_buffer_access("readlinkat", "write into", size, buf_size);
  return readlinkat(dirfd, path, buf, size);
}

ssize_t ABI_ATTR __readlink_chk_impl(const char* path, char* buf, size_t size, size_t buf_size) {
  __check_count("readlink", "size", size);
  __check_buffer_access("readlink", "write into", size, buf_size);
  return readlink(path, buf, size);
}

ssize_t ABI_ATTR __recvfrom_chk_impl(int socket, void* buf, size_t len, size_t buf_size,
                       int flags, sockaddr* src_addr, socklen_t* addrlen) {
  __check_buffer_access("recvfrom", "write into", len, buf_size);
  return recvfrom(socket, buf, len, flags, src_addr, addrlen);
}

ssize_t ABI_ATTR __sendto_chk_impl(int socket, const void* buf, size_t len, size_t buflen,
                     int flags, const struct sockaddr* dest_addr,
                     socklen_t addrlen) {
  __check_buffer_access("sendto", "read from", len, buflen);
  return sendto(socket, buf, len, flags, dest_addr, addrlen);
}

// Runtime implementation of __builtin____stpcpy_chk (used directly by compiler, not in headers)..
extern "C" char* ABI_ATTR __stpcpy_chk_impl(char* dst, const char* src, size_t dst_len) {
  // TODO: optimize so we don't scan src twice.
  size_t src_len = strlen(src) + 1;
  __check_buffer_access("stpcpy", "write into", src_len, dst_len);
  return stpcpy(dst, src);
}

// Runtime implementation of __builtin____stpncpy_chk (used directly by compiler, not in headers).
extern "C" char* ABI_ATTR __stpncpy_chk_impl(char* dst, const char* src, size_t len, size_t dst_len) {
  __check_buffer_access("stpncpy", "write into", len, dst_len);
  return stpncpy(dst, src, len);
}

// This is a variant of __stpncpy_chk, but it also checks to make
// sure we don't read beyond the end of "src". The code for this is
// based on the original version of stpncpy, but modified to check
// how much we read from "src" during the copy operation.
char* ABI_ATTR __stpncpy_chk2_impl(char* dst, const char* src, size_t n, size_t dst_len, size_t src_len) {
  __check_buffer_access("stpncpy", "write into", n, dst_len);
  if (n != 0) {
    char* d = dst;
    const char* s = src;

    do {
      size_t s_copy_len = static_cast<size_t>(s - src);
      if (__predict_false(s_copy_len >= src_len)) {
        __fortify_fatal("stpncpy: detected read past end of %zu-byte buffer", src_len);
      }

      if ((*d++ = *s++) == 0) {
        // NUL pad the remaining n-1 bytes.
        while (--n != 0) {
          *d++ = 0;
        }
        break;
      }
    } while (--n != 0);
  }

  return dst;
}

// strcat is performance-critical enough that we have assembler __strcat_chk implementations.
// This function is used to give better diagnostics than we can easily do from assembler.
extern "C" void ABI_ATTR __strcat_chk_fail_impl(size_t dst_buf_size) {
  __fortify_fatal("strcat: prevented write past end of %zu-byte buffer", dst_buf_size);
}

char* ABI_ATTR __strchr_chk_impl(const char* p, int ch, size_t s_len) {
  for (;; ++p, s_len--) {
    if (__predict_false(s_len == 0)) {
      __fortify_fatal("strchr: prevented read past end of buffer");
    }
    if (*p == static_cast<char>(ch)) {
      return const_cast<char*>(p);
    }
    if (*p == '\0') {
      return nullptr;
    }
  }
}

// strcpy is performance-critical enough that we have assembler __strcpy_chk implementations.
// This function is used to give better diagnostics than we can easily do from assembler.
extern "C" void ABI_ATTR __strcpy_chk_fail_impl(size_t dst_buf_size) {
  __fortify_fatal("strcpy: prevented write past end of %zu-byte buffer", dst_buf_size);
}

size_t ABI_ATTR __strlcat_chk_impl(char* dst, const char* src,
                     size_t supplied_size, size_t dst_len_from_compiler) {
  __check_buffer_access("strlcat", "write into", supplied_size, dst_len_from_compiler);
  return strlcat(dst, src, supplied_size);
}

size_t ABI_ATTR __strlcpy_chk_impl(char* dst, const char* src,
                     size_t supplied_size, size_t dst_len_from_compiler) {
  __check_buffer_access("strlcpy", "write into", supplied_size, dst_len_from_compiler);
  
  return strlcpy(dst, src, supplied_size);
}

size_t ABI_ATTR __strlen_chk_impl(const char* s, size_t s_len) {
  // TODO: "prevented" here would be a lie because this strlen can run off the end.
  // strlen is too important to be expensive, so we wanted to be able to call the optimized
  // implementation, but I think we need to implement optimized assembler __strlen_chk routines.
  size_t ret = strlen(s);
  if (__predict_false(ret >= s_len)) {
    __fortify_fatal("strlen: detected read past end of buffer");
  }
  return ret;
}

// Runtime implementation of __builtin____strncat_chk (used directly by compiler, not in headers).
extern "C" char* ABI_ATTR __strncat_chk_impl(char* dst, const char* src, size_t len, size_t dst_buf_size) {
  if (len == 0) {
    return dst;
  }

  size_t dst_len = __strlen_chk_impl(dst, dst_buf_size);
  char* d = dst + dst_len;
  dst_buf_size -= dst_len;

  while (*src != '\0') {
    *d++ = *src++;
    len--; dst_buf_size--;

    if (__predict_false(dst_buf_size == 0)) {
      __fortify_fatal("strncat: prevented write past end of buffer");
    }

    if (len == 0) {
      break;
    }
  }

  *d = '\0';
  return dst;
}

// Runtime implementation of __builtin____strncpy_chk (used directly by compiler, not in headers).
extern "C" char* ABI_ATTR __strncpy_chk_impl(char* dst, const char* src, size_t len, size_t dst_len) {
  __check_buffer_access("strncpy", "write into", len, dst_len);
  return strncpy(dst, src, len);
}

// This is a variant of __strncpy_chk, but it also checks to make
// sure we don't read beyond the end of "src". The code for this is
// based on the original version of strncpy, but modified to check
// how much we read from "src" during the copy operation.
char* ABI_ATTR __strncpy_chk2_impl(char* dst, const char* src, size_t n, size_t dst_len, size_t src_len) {
  __check_buffer_access("strncpy", "write into", n, dst_len);
  if (n != 0) {
    char* d = dst;
    const char* s = src;

    do {
      size_t s_copy_len = static_cast<size_t>(s - src);
      if (__predict_false(s_copy_len >= src_len)) {
        __fortify_fatal("strncpy: detected read past end of %zu-byte buffer", src_len);
      }

      if ((*d++ = *s++) == 0) {
        // NUL pad the remaining n-1 bytes.
        while (--n != 0) {
          *d++ = 0;
        }
        break;
      }
    } while (--n != 0);
  }

  return dst;
}

char* ABI_ATTR __strrchr_chk_impl(const char* p, int ch, size_t s_len) {
  for (const char* save = nullptr;; ++p, s_len--) {
    if (s_len == 0) {
      __fortify_fatal("strrchr: prevented read past end of buffer");
    }
    if (*p == static_cast<char>(ch)) {
      save = p;
    }
    if (!*p) {
      return const_cast<char*>(save);
    }
  }
}

mode_t ABI_ATTR __umask_chk_impl(mode_t mode) {
  if (__predict_false((mode & 0777) != mode)) {
    __fortify_fatal("umask: called with invalid mask %o", mode);
  }

  return umask(mode);
}

// Runtime implementation of __builtin____vsnprintf_chk (used directly by compiler, not in headers).
extern "C" int ABI_ATTR __vsnprintf_chk_impl(char* dst, size_t supplied_size, int /*flags*/,
                               size_t dst_len_from_compiler, const char* format, va_list va) {
  __check_buffer_access("vsnprintf", "write into", supplied_size, dst_len_from_compiler);
  return vsnprintf(dst, supplied_size, format, va);
}

// Runtime implementation of __builtin____snprintf_chk (used directly by compiler, not in headers).
extern "C" int ABI_ATTR __snprintf_chk_impl(char* dst, size_t supplied_size, int flags,
                              size_t dst_len_from_compiler, const char* format, ...) {
  va_list va;
  va_start(va, format);
  int result = __vsnprintf_chk_impl(dst, supplied_size, flags, dst_len_from_compiler, format, va);
  va_end(va);
  return result;
}

// Runtime implementation of __builtin____vsprintf_chk (used directly by compiler, not in headers).
extern "C" int ABI_ATTR __vsprintf_chk_impl(char* dst, int /*flags*/,
                              size_t dst_len_from_compiler, const char* format, va_list va) {
  // The compiler uses SIZE_MAX to mean "no idea", but our vsnprintf rejects sizes that large.
  int result = vsnprintf(dst,
                         dst_len_from_compiler == SIZE_MAX ? SSIZE_MAX : dst_len_from_compiler,
                         format, va);

  // Try to catch failures after the fact...
  __check_buffer_access("vsprintf", "write into", result + 1, dst_len_from_compiler);
  return result;
}

// Runtime implementation of __builtin____sprintf_chk (used directly by compiler, not in headers).
extern "C" int ABI_ATTR __sprintf_chk_impl(char* dst, int flags, size_t dst_len_from_compiler,
                             const char* format, ...) {
  va_list va;
  va_start(va, format);
  int result = __vsprintf_chk_impl(dst, flags, dst_len_from_compiler, format, va);
  va_end(va);
  return result;
}

ssize_t ABI_ATTR __write_chk_impl(int fd, const void* buf, size_t count, size_t buf_size) {
  __check_count("write", "count", count);
  __check_buffer_access("write", "read from", count, buf_size);
  return write(fd, buf, count);
}

// Runtime implementation of __builtin____strcat_chk (used directly by compiler, not in headers).
extern "C" char* ABI_ATTR __strcat_chk_impl(char* dst, const char* src, size_t dst_buf_size) {
  char* save = dst;
  size_t dst_len = __strlen_chk_impl(dst, dst_buf_size);

  dst += dst_len;
  dst_buf_size -= dst_len;

  while ((*dst++ = *src++) != '\0') {
    dst_buf_size--;
    if (__predict_false(dst_buf_size == 0)) {
      __fortify_fatal("strcat: prevented write past end of %zu-byte buffer", dst_buf_size);
    }
  }

  return save;
}

// Runtime implementation of __builtin____strcpy_chk (used directly by compiler, not in headers).
extern "C" char* ABI_ATTR __strcpy_chk_impl(char* dst, const char* src, size_t dst_len) {
  // TODO: optimize so we don't scan src twice.
  size_t src_len = strlen(src) + 1;
  __check_buffer_access("strcpy", "write into", src_len, dst_len);
  return strcpy(dst, src);
}

#if !defined(NO___MEMCPY_CHK)
// Runtime implementation of __memcpy_chk (used directly by compiler, not in headers).
extern "C" void* ABI_ATTR __memcpy_chk_impl(void* dst, const void* src, size_t count, size_t dst_len) {
  __check_count("memcpy", "count", count);
  __check_buffer_access("memcpy", "write into", count, dst_len);
  return memcpy(dst, src, count);
}
#endif // NO___MEMCPY_CHK

// Runtime implementation of __mempcpy_chk (used directly by compiler, not in headers).
extern "C" void* ABI_ATTR __mempcpy_chk_impl(void* dst, const void* src, size_t count, size_t dst_len) {
  __check_count("mempcpy", "count", count);
  __check_buffer_access("mempcpy", "write into", count, dst_len);
  return mempcpy(dst, src, count);
}
