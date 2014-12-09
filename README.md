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

if(!pool1 || !pool2) printf("Can't alloc more space\n");
```

### Allocate Memory

You can get a space of a certain size via function:

```c
void* xmem_alloc(xmem_pool_handle handle);
```

For an example:

```c
stct* my_stct = xmem_alloc(pool1);
if(!my_stct) printf("Can't alloc more space.\n");
```

And then you will get a **whole empty** space that fits `stct`.

### Recover Memory

After your scope you want to recover your memory, don't use `free()`. There's a function below:

```c
int xmem_free(xmem_pool_handle handle, void* pointer);
```

> The return value:
> + `0`: invalid pool handler or the pointer not belongs to this pool, can't be recovered.
> + `1`: succeeded.

For an example:

```c
int result = xmem_free(pool1, my_stct);
if(!result) printf("Cannot be recovered.\n");
```

