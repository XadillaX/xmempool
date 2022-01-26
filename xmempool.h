#ifndef XMEMPOOL_H_
#define XMEMPOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#include "__stdint.h"
#else
#include <stdint.h>
#endif

#define XMEMPOOL_VERSION_MAJOR 1
#define XMEMPOOL_VERSION_MINOR 1
#define XMEMPOOL_VERSION_PATCH 1
#define XMEMPOOL_VERSION_STRING "1.1.1"

typedef char* xmem_pool_handle;

// 5 APIs so far
//   not recommanded for multi-threaded.
//   if you want to use it for multi-threaded
//   now, please use one pool for one thread.
extern xmem_pool_handle xmem_create_pool(uint32_t block_size);
extern void xmem_destroy_pool(xmem_pool_handle pool);
extern char* xmem_alloc(xmem_pool_handle handle);
extern int xmem_free(xmem_pool_handle handle, char* pointer);
extern void xmem_print_info(xmem_pool_handle pool);
extern void xmem_clean_up();

#ifdef __cplusplus
}
#endif

#endif  // XMEMPOOL_H_
