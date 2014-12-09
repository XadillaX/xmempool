# X Memo Pool

A memory pool implemented by C.

## Usage

### Create Pool

At first you should create a pool handler for your data (or structure).

```c
xmem_pool_handler xmem_create_pool(unsigned int block_size);
```

> `xmem_pool_hander` is only a pointer which `typedef`ed at **xmempool.h**.
>
> ```c
> typedef void* xmem_pool_handler;
> ```

For an example:

```c
typedef struct stct {
    int id;
    char str[16];
} stct;

xmem_pool_handler pool1 = xmem_create_pool(sizeof(stct));
xmem_pool_handler pool2 = xmem_create_pool(sizeof(int));
```

### Alloc Memory

You can get a space of a certain size via function:

```c
void* xmem_alloc(xmem_pool_handle handle);
```

For an example:

```c
stct* my_stct = xmem_alloc(pool1);
```

And then you will get a **whole empty** space that fits `stct`.

