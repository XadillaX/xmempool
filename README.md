# X Memo Pool

[![Join the chat at https://gitter.im/XadillaX/xmempool](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/XadillaX/xmempool?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

A memory pool implemented by C.

> Sharing a same pool in multi-thread is not recommanded.

## Usage

### Create Pool

At first you should create a pool handler for your data (or structure).

```c
xmem_pool_handler xmem_create_pool(unsigned int block_size);
```

> `xmem_pool_hander` is only a pointer which `typedef`ed at **xmempool.h**.
>
> ```c
> typedef char* xmem_pool_handler;
> ```
>
> **Notice:** first I used `typedef void* xmem_pool_handler`, but for compatible, I changed it to `char*`.

For an example:

```c
typedef struct stct {
    int  id;
    char str[16];
} stct;

xmem_pool_handler pool1 = xmem_create_pool(sizeof(stct));
xmem_pool_handler pool2 = xmem_create_pool(sizeof(int));

if(!pool1 || !pool2) printf("Can't alloc more space\n");
```

### Allocate Memory

You can get a space of a certain size via function:

```c
char* xmem_alloc(xmem_pool_handle handle);
```

For an example:

```c
stct* my_stct = (stct*)xmem_alloc(pool1);
if(!my_stct) printf("Can't alloc more space.\n");
```

And then you will get a **whole empty** space that fits `stct`.

### Recover Memory

After your scope you want to recover your memory, don't use `free()`. There's a function below:

```c
int xmem_free(xmem_pool_handle handle, char* pointer);
```

> The return value:
> + `0`: invalid pool handler or the pointer not belongs to this pool, can't be recovered.
> + `1`: succeeded.

For an example:

```c
int result = xmem_free(pool1, my_stct);
if(!result) printf("Cannot be recovered.\n");
```

### Destroy the Whole Pool

```c
void xmem_destroy_pool(xmem_pool_handle pool);
```

> **Caution!** This function will destroy the whole pool, `free`ing all your memory allocated by that pool, even if you haven't `xmem_free` yet.

```c
xmem_destroy_pool(pool1);
xmem_destroy_pool(pool2);
```

### More

You can refer to [xmempool.h](xmempool.h) for any more functions.

## Performance

```shell
$ npm install
```

Install dependencies via command above and then type follow command:

```shell
$ node perf.js
```

The report will be generated at `./perf/report`.

[Here](http://blog.xcoder.in/xmempool/)'s some my report.

## Contribute

You're welcome to pull requests!

「雖然我覺得不怎麼可能有人會關注我」

