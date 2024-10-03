#ifndef __BIONIC_FILE__
#define __BIONIC_FILE__

#include <cstdint>
#include <cstddef>
#include <cstdio>

typedef off_t BIONIC_fpos_t;

/* "struct dirent" from bionic/libc/include/dirent.h */
struct bionic_dirent {
    uint64_t         d_ino;
    int64_t          d_off;
    unsigned short   d_reclen;
    unsigned char    d_type;
    char             d_name[256];
};

/* https://android.googlesource.com/platform/bionic/+/68dc20d41193831a94df04b994ff2f601dd38d10/libc/include/stdio.h */
/* stdio buffers */
#if defined(__LP64__)
struct BIONIC__sbuf {
  unsigned char* _base;
  size_t _size;
};
#else
struct BIONIC__sbuf {
	unsigned char *_base;
	int	_size;
};
#endif

typedef	struct BIONIC__sFILE {
	unsigned char *_p;	/* current position in (some) buffer */
	int	_r;		/* read space left for getc() */
	int	_w;		/* write space left for putc() */
#if defined(__LP64__)
	int	_flags;		/* flags, below; this FILE is free if 0 */
	int	_file;		/* fileno, if Unix descriptor, else -1 */
#else
	short	_flags;		/* flags, below; this FILE is free if 0 */
	short	_file;		/* fileno, if Unix descriptor, else -1 */
#endif
	struct	BIONIC__sbuf _bf;	/* the buffer (at least 1 byte, if !NULL) */
	int	_lbfsize;	/* 0 or -_bf._size, for inline putc */
	/* operations */
	void	*_cookie;	/* cookie passed to io functions */
	int	(*_close)(void *);
	int	(*_read)(void *, char *, int);
	BIONIC_fpos_t	(*_seek)(void *, BIONIC_fpos_t, int);
	int	(*_write)(void *, const char *, int);
	/* extension data, to avoid further ABI breakage */
	struct	BIONIC__sbuf _ext;
	/* data for long sequences of ungetc() */
	unsigned char *_up;	/* saved _p when _p is doing ungetc data */
	int	_ur;		/* saved _r when _r is counting ungetc data */
	/* tricks to meet minimum requirements even when malloc() fails */
	unsigned char _ubuf[3];	/* guarantee an ungetc() buffer */
	unsigned char _nbuf[1];	/* guarantee a getc() buffer */
	/* separate buffer for fgetln() when line crosses buffer boundary */
	struct	BIONIC__sbuf _lb;	/* buffer for fgetln() */
	/* Unix stdio files get aligned to block boundaries on fseek() */
	int	_blksize;	/* stat.st_blksize (may be != _bf._size) */
	BIONIC_fpos_t	_offset;	/* current lseek offset */
} BIONIC_FILE;

#endif