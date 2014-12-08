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

typedef struct xmem_pool_block {
    void*                   start;
    unsigned int            block_size;
    struct xmem_pool_block* next;
    unsigned char           is_block_start;
} xmem_pool_block;

typedef struct xmem_pool {
    unsigned int            block_size;
    void*                   start;
    void*                   end;
} xmem_pool;

#endif

