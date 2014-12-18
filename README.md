# X Memo Pool

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

### Test 1

```
Performance of "./perf_xmem"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 88934466.62 CPU cycles
  Average of free in one case: 34828792.76 CPU cycles
  Avarage of total in one case: 139718323.94 CPU cycles

Performance of "./perf_alloc"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 144649249.46 CPU cycles
  Average of free in one case: 269216562.16 CPU cycles
  Avarage of total in one case: 413865891.36 CPU cycles

Effect rate: (malloc - xmempool) / malloc
  Alloc: 38.52%
  Free:  87.06%
  Total: 66.24%
```

> This report was ran on my ***MacBook Air (11-inch, Mid 2013)***.
>
> + CPU: 1.3GHz Intel Core i5
> + Memo: 4GB 1600 MHz DDR3
> + System: OS X Yosemite 10.10.2

### Test 2

```
Performance of "./perf_xmem"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 88497438.3 CPU cycles
  Average of free in one case: 38895793.32 CPU cycles
  Avarage of total in one case: 145739467.06 CPU cycles

Performance of "./perf_alloc"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 112048273.8 CPU cycles
  Average of free in one case: 40331425.12 CPU cycles
  Avarage of total in one case: 152379754.92 CPU cycles

Effect rate: (malloc - xmempool) / malloc
  Alloc: 21.02%
  Free:  3.56%
  Total: 4.36%
```

> This report was ran on my work computer which is under ***Deepin***.
>
> + CPU: Dual Intel(R) Pentium(R) CPU G620 @ 2.60GHz
> + Memo: 4GB 1600 MHz DDR3 x 2
> + System: Deepin 2014 (3.13.0-30-generic)

### Test 3

```
Performance of "./perf_xmem"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 192807375.4 CPU cycles
  Average of free in one case: 68994110.58 CPU cycles
  Avarage of total in one case: 318962417.64 CPU cycles

Performance of "./perf_alloc"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 197417483.18 CPU cycles
  Average of free in one case: 59714845.4 CPU cycles
  Avarage of total in one case: 257132389.86 CPU cycles

Effect rate: (malloc - xmempool) / malloc
  Alloc: 2.34%
  Free:  -15.54%
  Total: -24.05%
```

> This report was ran on my own VPS which is under ***Ubuntu***.
>
> + CPU: Dual Intel(R) Xeon(R) CPU E5-2620 0 @ 2.00GHz
> + Memo: 1G
> + System: Ubuntu 12.04.2 LTS (3.5.0-23-generic)

### Test 4

```
Performance of "./perf_xmem"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 58532207.76 CPU cycles
  Average of free in one case: 21924212.08 CPU cycles
  Avarage of total in one case: 90753352.7 CPU cycles

Performance of "./perf_alloc"
  Test cases: 50
  Times of each case: 1000000
  Average of alloc in one case: 76495548.18 CPU cycles
  Average of free in one case: 28027848.44 CPU cycles
  Avarage of total in one case: 104523436.7 CPU cycles

Effect rate: (malloc - xmempool) / malloc
  Alloc: 23.48%
  Free:  21.78%
  Total: 13.17%
```

> This report was ran on my own notebook Lenovo Y500 which is under ***Ubuntu***.
>
> + CPU: 8 x Intel(R) Core(TM) i7-3630QM CPU @ 2.40GHz
> + Memo: 4GB 1600 MHz DDR3 x 2
> + System: Ubuntu 14.04.1 LTS (3.13.0-40-generic)


## Contribute

You're welcome to pull requests!

「雖然我覺得不怎麼可能有人會關注我」

