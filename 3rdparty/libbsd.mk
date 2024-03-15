BSD_SRC=\
	./3rdparty/libbsd/src/fpurge.c             \
	./3rdparty/libbsd/src/strtonum.c           \
	./3rdparty/libbsd/src/strlcpy.c            \
	./3rdparty/libbsd/src/setmode.c            \
	./3rdparty/libbsd/src/progname.c           \
	./3rdparty/libbsd/src/timeconv.c           \
	./3rdparty/libbsd/src/radixsort.c          \
	./3rdparty/libbsd/src/arc4random.c         \
	./3rdparty/libbsd/src/wcslcpy.c            \
	./3rdparty/libbsd/src/reallocf.c           \
	./3rdparty/libbsd/src/humanize_number.c    \
	./3rdparty/libbsd/src/fgetln.c             \
	./3rdparty/libbsd/src/strtoi.c             \
	./3rdparty/libbsd/src/funopen.c            \
	./3rdparty/libbsd/src/flopen.c             \
	./3rdparty/libbsd/src/freezero.c           \
	./3rdparty/libbsd/src/setproctitle.c       \
	./3rdparty/libbsd/src/fparseln.c           \
	./3rdparty/libbsd/src/getpeereid.c         \
	./3rdparty/libbsd/src/setproctitle_ctor.c  \
	./3rdparty/libbsd/src/fmtcheck.c           \
	./3rdparty/libbsd/src/dehumanize_number.c  \
	./3rdparty/libbsd/src/fgetwln.c            \
	./3rdparty/libbsd/src/recallocarray.c      \
	./3rdparty/libbsd/src/strnstr.c            \
	./3rdparty/libbsd/src/pwcache.c            \
	./3rdparty/libbsd/src/strmode.c            \
	./3rdparty/libbsd/src/explicit_bzero.c     \
	./3rdparty/libbsd/src/wcslcat.c            \
	./3rdparty/libbsd/src/readpassphrase.c     \
	./3rdparty/libbsd/src/vis.c                \
	./3rdparty/libbsd/src/unvis.c              \
	./3rdparty/libbsd/src/merge.c              \
	./3rdparty/libbsd/src/err.c                \
	./3rdparty/libbsd/src/expand_number.c      \
	./3rdparty/libbsd/src/closefrom.c          \
	./3rdparty/libbsd/src/nlist.c              \
	./3rdparty/libbsd/src/heapsort.c           \
	./3rdparty/libbsd/src/pidfile.c            \
	./3rdparty/libbsd/src/reallocarray.c       \
	./3rdparty/libbsd/src/strtou.c             \
	./3rdparty/libbsd/src/bsd_getopt.c         \
	./3rdparty/libbsd/src/stringlist.c         \
	./3rdparty/libbsd/src/strlcat.c            \
	./3rdparty/libbsd/src/inet_net_pton.c      \
	./3rdparty/libbsd/src/getbsize.c           \
	./3rdparty/libbsd/src/arc4random_uniform.c

MD_SRC=\
	./3rdparty/libmd/src/md5.c

#   Deprecated, use libmd
#	./3rdparty/libbsd/src/md5.c                \

BSD_OBJ=\
	$(patsubst ./3rdparty/libbsd/src/%.c,./build/${ARCH}/libbsd/%.c.o,${BSD_SRC}) \
	$(patsubst ./3rdparty/libmd/src/%.c,./build/${ARCH}/libmd/%.c.o,${MD_SRC})

BSD_CONFIG=-include 3rdparty/libbsd-config/libbsd.${ARCH}.h
BSD_FLAGS=\
	$(CFLAGS) \
	-DLIBBSD_OVERLAY \
	-DLIBBSD_DISABLE_DEPRECATED \
	-D__REENTRANT \
	-I./3rdparty/libbsd/include \
	-I./3rdparty/libbsd-config \
	-I./3rdparty/libbsd/include/bsd

MD_CONFIG=-include 3rdparty/libbsd-config/libmd.${ARCH}.h
MD_FLAGS=\
	$(CFLAGS) \
	-I./3rdparty/libbsd-config \
	-I./3rdparty/libmd/include

build/${ARCH}/libbsd/%.c.o: 3rdparty/libbsd/src/%.c
	@mkdir -p $(@D)
	$(CC) $(BSD_CONFIG) $(BSD_FLAGS) -c $< -o $@

build/${ARCH}/libmd/%.c.o: 3rdparty/libmd/src/%.c
	@mkdir -p $(@D)
	$(CC) $(MD_CONFIG) $(MD_FLAGS) -c $< -o $@