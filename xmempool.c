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
#include "xmempool.h"

#define ALLOC_BLOCK_NODE_COUNT 1024
xmem_pool_block* _free_block_ptr = 0;
xmem_pool_block* _free_block_ptr_end = 0;

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

