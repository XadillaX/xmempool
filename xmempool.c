/**
 * xmempool - A memory pool implemented by C.
 *
 * Key Points:
 * 1. xmem_pool_handle points to the first pool in the chain.
 * 2. Each xmem_pool allocates a large memory space, divided into fixed-size
 *    blocks.
 * 3. free_blocks in xmem_pool points to a linked list of xmem_pool_block
 *    structures.
 * 4. Each xmem_pool_block's start points to an actual memory block in the
 *    pool's space.
 * 5. Freed blocks are always returned to the first pool for optimal
 *    performance.
 * 6. New pools are created and chained when the current pool is full.
 */

#include "xmempool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif

typedef struct xmem_pool_block {
  char* start;
  uint32_t block_size;
  struct xmem_pool_block* next;
  uint32_t is_block_start;
} xmem_pool_block;

typedef struct xmem_pool {
  uint32_t block_size;
  uint32_t block_count;

  char* start;
  char* end;

  xmem_pool_block* free_blocks;
  xmem_pool_block* free_blocks_tail;
  struct xmem_pool* next;

  uint32_t next_alloc_length;
} xmem_pool;

#define MIN_ALLOC_BLOCK_NODE_COUNT (1024)
#define MAX_ALLOC_BLOCK_NODE_COUNT (1024 << 2)
#define MIN_ALLOC_LENGTH (1024)
#define MAX_ALLOC_LENGTH (1024 << 2)
static uint32_t _xmem_pool_next_alloc_block_node_count =
    MIN_ALLOC_BLOCK_NODE_COUNT;
static xmem_pool_block* _free_block_ptr = 0;
static xmem_pool_block* _free_block_ptr_end = 0;
static int _xmem_page_size = 0;
static uint32_t _block_size = sizeof(xmem_pool_block);

uint32_t size_count_with_4(uint32_t size) {
  if (size % 4 == 0) {
    return size;
  } else {
    return size + (4 - size % 4);
  }
}

/**
 * @brief Allocate memory for new block nodes
 *
 * This function allocates a chunk of memory for new block nodes when the
 * current pool is exhausted. It doubles the allocation size each time, up to
 * a maximum defined by MAX_ALLOC_BLOCK_NODE_COUNT. The newly allocated nodes
 * are then added to the free block list.
 */
static inline void _alloc_block_nodes() {
  uint32_t size_count = _block_size * _xmem_pool_next_alloc_block_node_count;
  _xmem_pool_next_alloc_block_node_count <<= 1;
  if (_xmem_pool_next_alloc_block_node_count > MAX_ALLOC_BLOCK_NODE_COUNT) {
    _xmem_pool_next_alloc_block_node_count = MAX_ALLOC_BLOCK_NODE_COUNT;
  }

#ifdef _WIN32
  char* start = (char*)malloc(size_count);
#else
  char* start;
  posix_memalign((void**)&start, _xmem_page_size, size_count);
#endif
  char* block_ptr = start;
  char* end = start + size_count;

  // Check if memory allocation was successful
  if (!block_ptr) return;

  for (; block_ptr < end; block_ptr += _block_size) {
    xmem_pool_block* block = (xmem_pool_block*)block_ptr;
    block->is_block_start = (block_ptr == start);
    block->start = 0;

    // Link the current block to the previous one, except for the first block
    if (block_ptr != start) {
      ((xmem_pool_block*)(block_ptr - _block_size))->next = block;
    }

    block->block_size = 0;
  }
  ((xmem_pool_block*)(end - _block_size))->next = 0;

  // Add newly allocated blocks to the free list
  if (_free_block_ptr && _free_block_ptr_end) {
    // Append new blocks to the existing free list
    _free_block_ptr_end->next = (xmem_pool_block*)start;
  } else {
    // Initialize the free list with the new blocks
    _free_block_ptr = (xmem_pool_block*)start;
  }

  _free_block_ptr_end = (xmem_pool_block*)(end - _block_size);
}

/**
 * @brief Get the next available block node
 *
 * This function returns the next available block node from the free list.
 * If the free list is empty, it calls _alloc_block_nodes() to allocate more.
 *
 * @return Pointer to the next available block node, or NULL if allocation fails
 */
static inline xmem_pool_block* _get_next_block_node() {
  xmem_pool_block* should_return;

  // If no more block node, generate a new block node list
  if (!_free_block_ptr) {
    _alloc_block_nodes();
  }

  // If still no more block node, return NULL to indicate error
  if (!_free_block_ptr) return (xmem_pool_block*)0;

  should_return = _free_block_ptr;
  _free_block_ptr = _free_block_ptr->next;

  // If should_return is the last one, set the tail to NULL
  if (should_return == _free_block_ptr_end) {
    _free_block_ptr_end = 0;
  }

  return should_return;
}

static inline void _recover_block_node(xmem_pool_block* node) {
  node->block_size = 0;
  node->next = NULL;
  node->start = NULL;

  if (_free_block_ptr && _free_block_ptr_end) {
    _free_block_ptr_end->next = node;
    _free_block_ptr_end = node;
  } else {
    _free_block_ptr = _free_block_ptr_end = node;
  }
}

static inline int _xmem_count_free_blocks(xmem_pool* pool) {
  xmem_pool_block* block;
  int count;

  if (!pool->free_blocks) return 0;
  block = pool->free_blocks;
  count = 1;

  while (block != pool->free_blocks_tail) {
    count++;
    block = block->next;
  }

  return count;
}

/**
 * @brief Create a new memory pool
 *
 * This function creates a new memory pool with the specified block size and
 * count. It allocates memory for the pool structure and the actual memory
 * blocks, then initializes the free block list.
 *
 * @param block_size Size of each memory block in the pool
 * @param block_count Number of blocks to allocate initially
 * @return Handle to the created pool, or NULL if creation failed
 */
xmem_pool_handle _create_pool(uint32_t block_size, uint32_t block_count) {
  static uint32_t _pool_size = sizeof(xmem_pool);
  xmem_pool* pool = (xmem_pool*)malloc(_pool_size);
  uint32_t size_count = block_count * block_size;
  uint32_t alloc_size = size_count_with_4(size_count);
  char* space;

#ifndef _WIN32
  if (_xmem_page_size < alloc_size) {
    posix_memalign((void**)&space, _xmem_page_size, alloc_size);
  } else {
#endif
    space = (char*)malloc(size_count);
#ifndef _WIN32
  }
#endif
  xmem_pool_block* start_block = 0;
  xmem_pool_block* end_block = 0;

  // Allocate memory for the pool structure
  if (!pool) return (char*)0;
  if (!space) {
    free(pool);
    return (char*)0;
  }

  // Initialize pool properties
  pool->start = space;
  pool->end = space + (block_count * block_size);

  pool->block_size = block_size;
  pool->block_count = block_count;
  pool->next = 0;

  pool->next_alloc_length = block_count << 1;
  if (pool->next_alloc_length > MAX_ALLOC_LENGTH) {
    pool->next_alloc_length = MAX_ALLOC_LENGTH;
  }

  // Initialize free blocks
  for (; space < pool->end; space += block_size) {
    xmem_pool_block* wrapper = _get_next_block_node();
    if (!start_block) {
      start_block = wrapper;
    }

    // Handle allocation failure
    if (!wrapper) {
      free(pool);
      free(space);
      while (start_block) {
        xmem_pool_block* next = start_block->next;
        _recover_block_node(start_block);
        start_block = next;
      }
      return 0;
    }

    wrapper->block_size = block_size;
    wrapper->start = space;
    wrapper->next = 0;

    // Link blocks in the free list
    if (end_block) {
      end_block->next = wrapper;
    }
    end_block = wrapper;
  }

  pool->free_blocks = start_block;
  pool->free_blocks_tail = end_block;

  return (char*)pool;
}

void xmem_print_info(xmem_pool_handle _pool) {
  xmem_pool* pool = (xmem_pool*)_pool;
  int pool_id = 0;

  if (!_pool) return;

  while (pool) {
    printf("----- POOL OF SIZE [%.4d] -----\n", pool->block_size);
    printf("  + id: %d\n", pool_id++);
    printf("  + count: %d\n", pool->block_count);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    printf("  + spaces: [0x%.8X, 0x%.8X)\n",
           (unsigned int)pool->start,
           (unsigned int)pool->end);
#pragma GCC diagnostic pop

    if (pool_id == 1) {
      printf("  + free blocks: %d\n", _xmem_count_free_blocks(pool));
    } else {
      printf("  + free blocks: -\n");
    }

    pool = pool->next;
  }
}

xmem_pool_handle xmem_create_pool(unsigned int block_size) {
#ifndef _WIN32
  // Initialize page size for non-Windows systems
  if (_xmem_page_size == 0) {
    _xmem_page_size = getpagesize();
  }
#endif

#ifdef XMEM_DBG
  // Debug mode: create pool and print creation message
  char* pool = _create_pool(block_size, MIN_ALLOC_LENGTH);
  printf("A new pool of [%d] is created!", block_size);
  return pool;
#endif

  // Normal mode: create pool with minimum allocation length
  return _create_pool(block_size, MIN_ALLOC_LENGTH);
}

void xmem_destroy_pool(xmem_pool_handle handle) {
  xmem_pool* pool = (xmem_pool*)handle;
  xmem_pool* next_pool;

  if (!handle) return;

  // Iterate through all pools in the chain
  while (pool) {
    // Free the memory space allocated for blocks
    free(pool->start);

    // For the first pool, recover all free block nodes
    if (pool == (xmem_pool*)handle) {
      xmem_pool_block* block = pool->free_blocks;
      xmem_pool_block* next_block;

      while (block) {
        next_block = block->next;
        _recover_block_node(block);
        block = next_block;
      }
    }

    // Move to the next pool
    next_pool = pool->next;
    free(pool);
    pool = next_pool;
  }
}

char* xmem_alloc(xmem_pool_handle handle) {
  static uint32_t pool_element_size = sizeof(xmem_pool);
  xmem_pool* pool = (xmem_pool*)handle;
  xmem_pool temp_pool;
  xmem_pool_block* block;
  char* space;

  // If no more free blocks, create a new pool
  if (!pool->free_blocks) {
    xmem_pool* new_pool =
        (xmem_pool*)_create_pool(pool->block_size, pool->next_alloc_length);

    if (!new_pool) {
      return 0;
    }

    // Swap the new pool with the current one
    memcpy(&temp_pool, pool, pool_element_size);
    memcpy(pool, new_pool, pool_element_size);
    memcpy(new_pool, &temp_pool, pool_element_size);

    pool->next = new_pool;
  }

  // Allocate the first free block
  block = pool->free_blocks;
  pool->free_blocks = block->next;
  space = block->start;

  if (!pool->free_blocks) pool->free_blocks_tail = 0;

  // Initialize allocated space and recover the block node
  memset(space, 0, block->block_size);
  _recover_block_node(block);

  return space;
}

int xmem_free(xmem_pool_handle handle, char* pointer) {
  xmem_pool* pool = (xmem_pool*)handle;

  // Get a new block node
  xmem_pool_block* block = _get_next_block_node();
  if (!block) {
    return 0;
  }

  // Initialize the new block and add it to the free list
  block->block_size = pool->block_size;
  block->start = pointer;
  block->next = 0;

  if (!pool->free_blocks && !pool->free_blocks_tail) {
    pool->free_blocks = pool->free_blocks_tail = block;
    return 1;
  }

  pool->free_blocks_tail->next = block;
  pool->free_blocks_tail = block;

  return 1;
}

void xmem_init() {
  // Placeholder for potential initialization code
}

void xmem_clean_up() {
  xmem_pool_block* blk = _free_block_ptr;
  xmem_pool_block* start = 0;
  xmem_pool_block* end = 0;

  // Reorganize the free block list
  while (blk != 0) {
    if (blk->is_block_start) {
      if (end) {
        end->next = blk;
      } else {
        start = blk;
      }
      end = blk;
    }
    blk = blk->next;
  }
  if (end) end->next = 0;

  // Free all allocated block nodes
  blk = start;
  while (blk != 0) {
    end = blk->next;  // Temporary variable for the next block
    free(blk);
    blk = end;
  }

  // Reset global pointers
  _free_block_ptr = _free_block_ptr_end = 0;
}
