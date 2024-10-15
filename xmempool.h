#ifndef XMEMPOOL_H_
#define XMEMPOOL_H_

/**
 * @file xmempool.h
 * @brief A memory pool implemented by C.
 *
 * @warning THREAD SAFETY NOTICE:
 * Individual pools are not thread-safe. For multi-threaded use, create separate
 * pools for each thread.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#include "__stdint.h"
#else
#include <stdint.h>
#endif

/**
 * @brief Major version number of xmempool
 */
#define XMEMPOOL_VERSION_MAJOR 1

/**
 * @brief Minor version number of xmempool
 */
#define XMEMPOOL_VERSION_MINOR 1

/**
 * @brief Patch version number of xmempool
 */
#define XMEMPOOL_VERSION_PATCH 2

/**
 * @brief Version string of xmempool
 */
#define XMEMPOOL_VERSION_STRING "1.1.2"

/**
 * @brief Handle type for memory pools
 *
 * This is a pointer to the first pool in the chain of memory pools. It's used
 * as an opaque handle to interact with the memory pool system.
 */
typedef char* xmem_pool_handle;

/**
 * @brief Create a new memory pool
 *
 * This function creates the initial memory pool with the specified block size.
 * It allocates a large chunk of memory and divides it into fixed-size blocks.
 * Additional pools will be created automatically as needed.
 *
 * @param block_size Size of each block in the pool
 * @return Handle to the created pool, or NULL if creation failed
 */
extern xmem_pool_handle xmem_create_pool(uint32_t block_size);

/**
 * @brief Destroy a memory pool and free all associated resources
 *
 * This function frees all memory associated with the pool, including all
 * chained pools and their blocks. It also recovers all block nodes used for
 * managing free blocks.
 *
 * @param pool Handle to the pool to be destroyed
 */
extern void xmem_destroy_pool(xmem_pool_handle pool);

/**
 * @brief Allocate a block from the memory pool
 *
 * This function returns a pointer to a free block from the pool. If the current
 * pool is full, it automatically creates and chains a new pool. The allocated
 * block is always zero-initialized.
 *
 * @param handle Handle to the pool to allocate from
 * @return Pointer to the allocated block, or NULL if allocation failed
 */
extern char* xmem_alloc(xmem_pool_handle handle);

/**
 * @brief Free a block back to the memory pool
 *
 * This function returns a block to the free list of the first pool in the
 * chain. It does not actually free memory, but makes the block available for
 * future allocations.
 *
 * @param handle Handle to the pool
 * @param pointer Pointer to the block to be freed
 * @return 1 if successful, 0 if failed (e.g., if no free block nodes are
 * available)
 */
extern int xmem_free(xmem_pool_handle handle, char* pointer);

/**
 * @brief Print information about the memory pool
 *
 * This function prints detailed information about each pool in the chain,
 * including its size, block count, memory range, and number of free blocks.
 *
 * @param pool Handle to the pool
 */
extern void xmem_print_info(xmem_pool_handle pool);

/**
 * @brief Clean up global resources used by xmempool
 *
 * This function frees all global resources used by the xmempool system,
 * particularly the memory used for managing block nodes. It should be called
 * when the xmempool system is no longer needed.
 */
extern void xmem_clean_up();

#ifdef __cplusplus
}
#endif

#endif  // XMEMPOOL_H_
