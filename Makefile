OBJ = xmempool.o
TESTS = xmempool-test
LIBNAME = xmempool

CC = gcc
OPTIMIZATION ?= -O2
WARNINGS = -Wno-missing-field-initializers
DEBUG ?= -g -ggdb

REAL_CFLAGS = $(OPTIMIZATION) -fPIC $(CFLAGS) $(WARNINGS) $(DEBUG)

DYLIBNAME = $(LIBNAME).so
DYLIB_MAKE_CMD = $(CC) -shared -o $(DYLIBNAME) $(LDFLAGS)
STLIBNAME = $(LIBNAME).a
STLIB_MAKE_CMD = ar rcs $(STLIBNAME)

all: $(DYLIBNAME)

$(DYLIBNAME): $(OBJ)
	$(DYLIB_MAKE_CMD) $(OBJ)

$(STLIBNAME): $(OBJ)
	$(STLIB_MAKE_CMD) $(OBJ)

dynamic: $(DYLIBNAME)
static: $(STLIBNAME)

test: test.o $(STLIBNAME)
	$(CC) -o $(TESTS) $(REAL_CFLAGS) $< -I. $(STLIBNAME)

xmempool.o: xmempool.c
test.o: test.c

%.o: %.c
	$(CC) -c $(REAL_CFLAGS) $<

.PHONY: all test clean

clean:
	$(RM) -rf *.o $(DYLIBNAME) $(STLIBNAME) $(TESTS) output/*

