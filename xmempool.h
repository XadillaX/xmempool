/*
 * =====================================================================================
 *
 *       Filename:  xmempool.h
 *
 *    Description:  XadillaX' memory pool for C.
 *
 *        Version:  1.0
 *        Created:  2014/12/08 11时41分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  XadillaX, 
 *   Organization:  Huaban.com
 *
 * =====================================================================================
 */
#ifndef __XMEMPOOL_H__
#define __XMEMPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

typedef char*           xmem_pool_handle;

// 5 APIs so far
//   not recommanded for multi-threaded.
//   if you want to use it for multi-threaded
//   now, please use one pool for one thread.
extern xmem_pool_handle xmem_create_pool(uint32_t block_size);
extern void             xmem_destroy_pool(xmem_pool_handle pool);
extern char*            xmem_alloc(xmem_pool_handle handle);
extern int              xmem_free(xmem_pool_handle handle, char* pointer);
extern void             xmem_print_info(xmem_pool_handle pool);

#ifdef __cplusplus
}
#endif

#endif

