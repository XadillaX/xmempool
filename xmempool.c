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

#define ALLOC_BLOCK_NODE_COUNT  1024
#define MIN_ALLOC_LENGTH        1024
xmem_pool_block*    _free_block_ptr     = 0;
xmem_pool_block*    _free_block_ptr_end = 0;

void _alloc_block_nodes()
{
    static unsigned int _block_size = sizeof(xmem_pool_block);
    unsigned int        size_count  = _block_size * ALLOC_BLOCK_NODE_COUNT;

    void* start;
    void* block_ptr = start = malloc(size_count);
    void* end       = start + size_count;

    // can't malloc more space(s).
    if(!block_ptr) return;

    for(; block_ptr < end; block_ptr += _block_size)
    {
        xmem_pool_block* block  = (xmem_pool_block*)block_ptr;
        block->is_block_start   = (block_ptr == start);
        block->start            = 0;

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
        _free_block_ptr_end->next   = (xmem_pool_block*)start;
    }
    else
    {
        // otherwise, replace the whole `_free_block_ptr`.
        _free_block_ptr             = start;
    }

    _free_block_ptr_end             = (xmem_pool_block*)(end - _block_size);
}

xmem_pool_block* _get_next_block_node()
{
    // if no more block node,
    // generate a new block node list
    if(!_free_block_ptr)
    {
        _alloc_block_nodes();
    }

    // if still no more block node,
    // return 0 to stand for error
    if(!_free_block_ptr) return (xmem_pool_block*)0;

    xmem_pool_block* should_return  = _free_block_ptr;
    _free_block_ptr                 = _free_block_ptr->next;

    // if `should_return` is the last one,
    // we should set the tail to 0.
    if(should_return == _free_block_ptr_end)
    {
        _free_block_ptr_end         = 0;
    }

    return should_return;
}

void _recover_block_node(xmem_pool_block* node)
{
    node->start         = 0;
    node->next          = 0;
    node->block_size    = 0;

    if(_free_block_ptr && _free_block_ptr_end)
    {
        _free_block_ptr_end->next = node;
        _free_block_ptr_end = node;
    }
    else
    {
        _free_block_ptr = _free_block_ptr_end = node;
    }
}

xmem_pool_handle _create_pool(unsigned int block_size,
        unsigned int block_count)
{
    static unsigned int _pool_size  = sizeof(xmem_pool);
    xmem_pool*          pool        = (xmem_pool*)malloc(_pool_size);
    void*               space       = malloc(block_count * block_size);

    // can't malloc more space
    if(!pool) return (void*)0;
    if(!space)
    {
        free(pool);
        return (void*)0;
    }

    // set the pool space's start and end
    pool->start         = space;
    pool->end           = space + (block_count * block_size);

    pool->block_size    = block_size;
    pool->block_count   = block_count;
    pool->next          = 0;

    // create free nodes!
    xmem_pool_block* start_block    = 0;
    xmem_pool_block* end_block      = 0;
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
        wrapper->start = space;
        wrapper->next = 0;

        // concat wrapper to the tail of block list
        if(end_block) end_block->next = wrapper;
        end_block = wrapper;
    }

    pool->free_blocks = start_block;

    return (void*)pool;
}

xmem_pool_handle create_pool(unsigned int block_size)
{
    return _create_pool(block_size, MIN_ALLOC_LENGTH);
}

void* xmem_alloc(xmem_pool_handle handle)
{
    xmem_pool* pool = (xmem_pool*)handle;

    // find a pool that has free node
    while(!pool->free_blocks && pool->next != 0)
    {
        pool = pool->next;
    }

    // if no more space, we create a new pool
    if(!pool->free_blocks && pool->next == 0)
    {
        xmem_pool* next_pool    = (xmem_pool*)_create_pool(pool->block_size, pool->block_count * 2);
        if(!next_pool)
        {
            return 0;
        }

        pool->next              = next_pool;
        pool                    = next_pool;
    }

    // get the first free space
    xmem_pool_block* block      = pool->free_blocks;
    pool->free_blocks           = block->next;
    void* space                 = block->start;

    // initialize space data & recover block node
    memset(space, 0, block->block_size);
    _recover_block_node(block);

    return space;
}

