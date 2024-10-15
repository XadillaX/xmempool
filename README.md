# X Memo Pool

A memory pool implemented by C.

## Overview

X Memo Pool is a lightweight and efficient memory management solution designed to optimize memory allocation and deallocation in C programs. It provides a pool-based approach to memory management, which can significantly improve performance in scenarios where frequent allocations of fixed-size blocks are required.

> Note: Sharing a single pool across multiple threads is not recommended for thread safety.

## Features

- Fast allocation and deallocation of fixed-size memory blocks
- Automatic pool expansion when the current pool is exhausted
- Zero-initialization of allocated blocks
- Easy-to-use API with creation, allocation, freeing, and destruction functions
- Debugging support with pool information printing

## Usage

### Creating a Pool

To start using X Memo Pool, first create a pool handler for your data structure:

```c
xmem_pool_handle xmem_create_pool(uint32_t block_size);
```

Example:

```c
typedef struct {
    int  id;
    char str[16];
} my_struct;

xmem_pool_handle pool = xmem_create_pool(sizeof(my_struct));
if (!pool) {
    printf("Failed to create memory pool\n");
}
```

### Allocating Memory

To allocate a block from the pool:

```c
char* xmem_alloc(xmem_pool_handle handle);
```

Example:

```c
my_struct* data = (my_struct*)xmem_alloc(pool);
if (!data) {
    printf("Failed to allocate memory from pool\n");
}
```

### Freeing Memory

To return a block to the pool:

```c
int xmem_free(xmem_pool_handle handle, char* pointer);
```

Example:

```c
int result = xmem_free(pool, (char*)data);
if (!result) {
    printf("Failed to free memory\n");
}
```

### Destroying the Pool

To destroy the entire pool and free all associated resources:

```c
void xmem_destroy_pool(xmem_pool_handle pool);
```

Example:

```c
xmem_destroy_pool(pool);
```

### Additional Functions

For more detailed information about the API, please refer to the [xmempool.h](xmempool.h) header file.

## Performance

To run performance tests:

1. Install dependencies:
   ```
   npm install
   ```

2. Run the performance script:
   ```
   node perf.js
   ```

The performance report will be generated in the `./perf/report` directory.

## Contributing

Contributions to X Memo Pool are welcome! Feel free to submit pull requests or open issues for bugs, feature requests, or improvements.

## License

X Memo Pool is open-source software. Please refer to the [LICENSE](LICENSE) file for detailed licensing information.
