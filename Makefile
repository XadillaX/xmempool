OBJ = xmempool.o
OBJ_D = xmempool.d
TESTS = xmempool-test
LIBNAME = xmempool
PERF = perf

CC = gcc
OPTIMIZATION ?= -O2
WARNINGS = -Wno-missing-field-initializers
DEBUG ?= -g -ggdb 

REAL_CFLAGS = $(OPTIMIZATION) -fPIC $(CFLAGS) $(WARNINGS) $(DEBUG)

# Dynamic lib name
DYLIBNAME = $(LIBNAME).so
# Dynamic lib make command
DYLIB_MAKE_CMD = $(CC) -shared -o $(DYLIBNAME) $(LDFLAGS)

# Static lib name
STLIBNAME = $(LIBNAME).a
# Static lib make command
STLIB_MAKE_CMD = ar rcs $(STLIBNAME)

# Static debug lib name
STLIBDNAME = $(LIBNAME)_d.a
# Static debug lib make command
STLIBD_MAKE_CMD = ar rcs $(STLIBDNAME)

all: $(DYLIBNAME)

$(DYLIBNAME): $(OBJ)
	$(DYLIB_MAKE_CMD) $(OBJ)

$(STLIBNAME): $(OBJ)
	$(STLIB_MAKE_CMD) $(OBJ)

$(STLIBDNAME): $(OBJ_D)
	$(STLIBD_MAKE_CMD) $(OBJ_D)

dynamic: $(DYLIBNAME)
static: $(STLIBNAME)

test_d: test.o $(STLIBDNAME)
	$(CC) -o $(TESTS)-d $(REAL_CFLAGS) $< -I. $(STLIBDNAME)

test: test.o $(STLIBNAME)
	$(CC) -o $(TESTS) $(REAL_CFLAGS) $< -I. $(STLIBNAME)

perf_xmem: perf/xmem.o $(STLIBNAME)
	$(CC) -o $(PERF)_xmem $(REAL_CFLAGS) $< -I. $(STLIBNAME)
perf_alloc: perf/alloc.o $(STLIBNAME)
	$(CC) -o $(PERF)_alloc $(REAL_CFLAGS) $< -I. $(STLIBNAME)


xmempool.d: xmempool.c

xmempool.o: xmempool.c

test.o: test.c

perf/xmem.o: perf/xmem.c perf/common.h
perf/alloc.o: perf/alloc.c perf/common.h

%.d: %.c
	$(CC) -c $(REAL_CFLAGS) -D XMEM_DBG $< -o $@

%.o: %.c
	$(CC) -c $(REAL_CFLAGS) $< -o $@

.PHONY: all test clean

clean:
	$(RM) -rf *.o *.d $(DYLIBNAME) $(STLIBDNAME) $(STLIBNAME) $(TESTS) $(TESTS)-d

