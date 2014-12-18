/*
 * =====================================================================================
 *
 *       Filename:  xmempool.c
 *
 *    Description:  XadillaX' memory pool for C.
 *
 *        Version:  1.0
 *        Created:  2014/12/08 11时41分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  XadillaX
 *   Organization:  Huaban.com
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmempool.h"

typedef struct xmem_pool_block {
    char*    start;
    uint32_t block_size;
    struct   xmem_pool_block* next;
    uint8_t  is_block_start;
} xmem_pool_block;

typedef struct xmem_pool {
    uint32_t          block_size;
    uint32_t          block_count;

    char*             start;
    char*             end;

    xmem_pool_block*  free_blocks;
    xmem_pool_block*  free_blocks_tail;
    struct xmem_pool* next;
} xmem_pool;

#define ALLOC_BLOCK_NODE_COUNT  (1024)
#define MIN_ALLOC_LENGTH        (1024)
xmem_pool_block* _free_block_ptr     = 0;
xmem_pool_block* _free_block_ptr_end = 0;

void _alloc_block_nodes()
{
    static uint32_t _block_size = sizeof(xmem_pool_block);
    uint32_t        size_count  = _block_size * ALLOC_BLOCK_NODE_COUNT;

    char* start;
    char* block_ptr = start = malloc(size_count);
    char* end       = start + size_count;

    // can't malloc more space(s).
    if(!block_ptr) return;

    for(; block_ptr < end; block_ptr += _block_size)
    {
        xmem_pool_block* block = (xmem_pool_block*)block_ptr;
        block->is_block_start  = (block_ptr == start);
        block->start           = 0;

        // if it's not starting block,
        // set previous block's next block
        // to this.
        if(block_ptr != start)
        {
            ((xmem_pool_block*)(block_ptr - _block_size))->next = block;
        }

        block->block_size = 0;
    }
    ((xmem_pool_block*)(end - _block_size))->next = 0;

    if(_free_block_ptr && _free_block_ptr_end)
    {
        // there's block pointer(s) before,
        // and still some in the `_free_block_ptr`, 
        // concat newer one list to the old one's tail.
        _free_block_ptr_end->next = (xmem_pool_block*)start;
    }
    else
    {
        // otherwise, replace the whole `_free_block_ptr`.
        _free_block_ptr = (xmem_pool_block*)start;
    }

    _free_block_ptr_end = (xmem_pool_block*)(end - _block_size);
}

xmem_pool_block* _get_next_block_node()
{
    xmem_pool_block* should_return;

    // if no more block node,
    // generate a new block node list
    if(!_free_block_ptr)
    {
        _alloc_block_nodes();
    }

    // if still no more block node,
    // return 0 to stand for error
    if(!_free_block_ptr) return (xmem_pool_block*)0;

    should_return   = _free_block_ptr;
    _free_block_ptr = _free_block_ptr->next;

    // if `should_return` is the last one,
    // we should set the tail to 0.
    if(should_return == _free_block_ptr_end)
    {
        _free_block_ptr_end = 0;
    }

    return should_return;
}

void _recover_block_node(xmem_pool_block* node)
{
    node->start      = 0;
    node->next       = 0;
    node->block_size = 0;

    if(_free_block_ptr && _free_block_ptr_end)
    {
        _free_block_ptr_end->next = node;
        _free_block_ptr_end       = node;
    }
    else
    {
        _free_block_ptr = _free_block_ptr_end = node;
    }
}

int _xmem_count_free_blocks(xmem_pool* pool)
{
    xmem_pool_block* block;
    int              count;

    if(!pool->free_blocks) return 0;
    block = pool->free_blocks;
    count = 1;

    while(block != pool->free_blocks_tail)
    {
        count++;
        block = block->next;
    }

    return count;
}

xmem_pool_handle _create_pool(uint32_t block_size, uint32_t block_count)
{
    static uint32_t  _pool_size  = sizeof(xmem_pool);
    xmem_pool*       pool        = (xmem_pool*)malloc(_pool_size);
    char*            space       = malloc(block_count * block_size);
    xmem_pool_block* start_block = 0;
    xmem_pool_block* end_block   = 0;

    // can't malloc more space
    if(!pool) return (char*)0;
    if(!space)
    {
        free(pool);
        return (char*)0;
    }

    // set the pool space's start and end
    pool->start       = space;
    pool->end         = space + (block_count * block_size);

    pool->block_size  = block_size;
    pool->block_count = block_count;
    pool->next        = 0;

    // create free nodes!
    for(; space < pool->end; space += block_size)
    {
        xmem_pool_block* wrapper = _get_next_block_node();
        if(!start_block)
        {
            start_block = wrapper; 
        }

        // if we can't create any more wrapper,
        // free `pool`, `space`,
        // and recover all pool blocks genereated before
        if(!wrapper)
        {
            free(pool);
            free(space);
            while(start_block)
            {
                xmem_pool_block* next = start_block->next;
                _recover_block_node(start_block);
                start_block = next;
            }
            return 0;
        }

        wrapper->block_size = block_size;
        wrapper->start      = space;
        wrapper->next       = 0;

        // concat wrapper to the tail of block list
        if(end_block) 
        {
            end_block->next = wrapper;
        }
        end_block = wrapper;
    }

    pool->free_blocks      = start_block;
    pool->free_blocks_tail = end_block;

    return (char*)pool;
}

void xmem_print_info(xmem_pool_handle _pool)
{
    xmem_pool* pool = (xmem_pool*)_pool;
    int pool_id     = 0;

    if(!_pool) return;

    while(pool)
    {
        printf("----- POOL OF SIZE [%.4d] -----\n", pool->block_size);
        printf("  + id: %d\n",                      pool_id++);
        printf("  + count: %d\n",                   pool->block_count);
        printf("  + spaces: [0x%.8X, 0x%.8X)\n",    (unsigned int)pool->start,
                                                    (unsigned int)pool->end);

        if(pool_id == 1)
        {
            printf("  + free blocks: %d\n", _xmem_count_free_blocks(pool));
        }
        else
        {
            printf("  + free blocks: -\n");
        }

        pool = pool->next;
    }
}

xmem_pool_handle xmem_create_pool(unsigned int block_size)
{
#ifdef XMEM_DBG
    char* pool = _create_pool(block_size, MIN_ALLOC_LENGTH);
    printf("A new pool of [%d] is created!", block_size);
    return pool;
#endif

    return _create_pool(block_size, MIN_ALLOC_LENGTH);
}

void xmem_destroy_pool(xmem_pool_handle handle)
{
    xmem_pool* pool = (xmem_pool*)handle;
    xmem_pool* next_pool;

    if(!handle) return;

    // from pool head to pool tail
    while(pool)
    {
        // free the space.
        // and then remove all free blocks object
        // from head to tail.
        free(pool->start);

        // all the blocks are mansged in the
        // first pool.
        if(pool == (xmem_pool*)handle)
        {
            xmem_pool_block* block = pool->free_blocks;
            xmem_pool_block* next_block;

            // from block head to block tail
            while(block)
            {
                next_block = block->next;
                _recover_block_node(block);
                block = next_block;
            }
        }

        // move to next pool
        next_pool = pool->next;
        free(pool);
        pool = next_pool;
    }

    return;
}

char* xmem_alloc(xmem_pool_handle handle)
{
    static uint32_t pool_element_size = sizeof(xmem_pool);
    xmem_pool* pool                   = (xmem_pool*)handle;
    xmem_pool temp_pool;
    xmem_pool_block* block;
    char* space;

    // if no more space, we create a new pool
    if(!pool->free_blocks)
    {
        xmem_pool* new_pool = (xmem_pool*)_create_pool(
                pool->block_size, 
                pool->block_count << 1);

        if(!new_pool)
        {
            return 0;
        }
        
        memcpy(&temp_pool, pool,       pool_element_size);
        memcpy(pool,       new_pool,   pool_element_size);
        memcpy(new_pool,   &temp_pool, pool_element_size);

        pool->next = new_pool;

        // ** 我太特么聪明了 **
        //
        // no matter how many pools here, all free
        // blocks are managed in the first pool.
        //
        // so we needn't do anything here.
    }

    // get the first free space
    block             = pool->free_blocks;
    pool->free_blocks = block->next;
    space             = block->start;

    if(!pool->free_blocks) pool->free_blocks_tail = 0;

    // initialize space data & recover block node
    memset(space, 0, block->block_size);
    _recover_block_node(block);

    return space;
}

int xmem_free(xmem_pool_handle handle, char* pointer)
{
    xmem_pool* pool = (xmem_pool*)handle;

    // get a block.
    // if no block, return false.
    xmem_pool_block* block = _get_next_block_node();
    if(!block)
    {
        return 0;
    }

    // create a new block to the free list.
    block->block_size = pool->block_size;
    block->start      = pointer;
    block->next       = 0;

    if(!pool->free_blocks && !pool->free_blocks_tail)
    {
        pool->free_blocks = pool->free_blocks_tail = block;
        return 1;
    }

    pool->free_blocks_tail->next = block;
    pool->free_blocks_tail       = block;

    return 1;
}

