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

typedef void*               xmem_pool_handle;

// 5 APIs so far
extern xmem_pool_handle     xmem_create_pool(unsigned int block_size);
extern void                 xmem_destroy_pool(xmem_pool_handle pool);
extern void*                xmem_alloc(xmem_pool_handle handle);
extern int                  xmem_free(xmem_pool_handle handle, void* pointer);
extern void                 xmem_print_info(xmem_pool_handle pool);

#ifdef __cplusplus
}
#endif

#endif

