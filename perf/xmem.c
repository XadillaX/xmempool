/*
 * =====================================================================================
 *
 *       Filename:  xmem.c
 *
 *    Description:  alloc of xmempool.
 *
 *        Version:  1.0
 *        Created:  2014/12/15 17时05分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  XadillaX
 *   Organization:  Huaban
 *
 * =====================================================================================
 */
#include "../xmempool.h"
#include "common.h"

xmem_pool_handle    pool = NULL;
test_struct*        test_array[TEST_COUNT];

int                 total = 0;
int                 alloc_total = 0;
int                 free_total = 0;

int main()
{
    pool = xmem_create_pool(sizeof(test_struct));

    int total_start, alloc_start, free_start;

    total_start = alloc_start = rdtsc();
    for(int i = 0; i < TEST_COUNT; i++)
    {
        test_array[i] = xmem_alloc(pool);
    }
    alloc_total = rdtsc() - alloc_start;

    free_start = rdtsc();
    for(int i = 0; i < TEST_COUNT; i++)
    {
        xmem_free(pool, test_array[i]);
    }
    free_total = rdtsc() - free_start;
    xmem_destroy_pool(pool);

    total = rdtsc() - total_start;

    printf("=== perf of xmem\n");
    printf("alloc & free times: %d\n", TEST_COUNT);
    printf("alloc time: %d CPU cycles\n", alloc_total);
    printf("free time: %d CPU cycles\n", free_total);
    printf("total time: %d CPU cycles\n", total);

    return 0;
}

