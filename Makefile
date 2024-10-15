OBJ = xmempool.o
OBJ_D = xmempool.d
TESTS = xmempool-test
LIBNAME = xmempool
PERF = perf

CC = gcc
OPTIMIZATION ?= -O2
WARNINGS = -Wno-missing-field-initializers -Wno-int-conversion
DEBUG ?= -g -ggdb

REAL_CFLAGS = $(OPTIMIZATION) -fPIC $(CFLAGS) $(WARNINGS) $(DEBUG) -std=c99

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

all: $(DYLIBNAME) perf_xmem perf_alloc test

%.compdb_entry: %.c
	@echo "    {" > $@
	@echo "        \"command\": \"$(CC) $(REAL_CFLAGS) -c $<\","   >> $@
	@echo "        \"directory\": \"$(CURDIR)\","               >> $@
	@echo "        \"file\": \"$<\""                    >> $@
	@echo "    },"                              >> $@

COMPDB_ENTRIES = $(addsuffix .compdb_entry, $(basename xmempool.c test.c perf/alloc.c perf/xmem.c))

compile_commands.json: $(COMPDB_ENTRIES)
	@echo "[" > $@.tmp
	@cat $^ >> $@.tmp
	@sed '$$d' < $@.tmp > $@
	@echo "    }" >> $@
	@echo "]" >> $@
	@rm $(COMPDB_ENTRIES)
	@rm $@.tmp

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

node_modules: package.json
	npm install; rm package-lock.json # I HATE package-lock.json!

.PYONY: clang-format cpplint
clang-format: .clang-format-stamp
.clang-format-stamp: xmempool.h xmempool.c test.c perf/xmem.c perf/alloc.c perf/common.h
	./node_modules/.bin/clang-format -i --style=file $?
	touch $@

cpplint: .cpplint-stamp
.cpplint-stamp: xmempool.h xmempool.c test.c perf/xmem.c perf/alloc.c perf/common.h
	./tools/cpplint/cpplint.py $?
	touch $@

%.d: %.c
	$(CC) -c $(REAL_CFLAGS) -D XMEM_DBG $< -o $@

%.o: %.c
	$(CC) -c $(REAL_CFLAGS) $< -o $@

.PHONY: all test clean

clean:
	$(RM) -rf *.o *.d $(DYLIBNAME) $(STLIBDNAME) $(STLIBNAME) $(TESTS) $(TESTS)-d perf/*.o perf_xmem perf_alloc

# Doxygen documentation generation
docs:
	doxygen Doxyfile

.PHONY: docs
