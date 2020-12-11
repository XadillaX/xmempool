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
 *
 * =====================================================================================
 */
#include "../xmempool.h"
#include "common.h"

xmem_pool_handle pool = NULL;

uint64_t total       = 0;
uint64_t alloc_total = 0;
uint64_t free_total  = 0;

int main(int argc, char** argv)
{
    deal_with_test_count(argc, argv);

    pool = xmem_create_pool(sizeof(test_struct));

    uint64_t total_start, alloc_start, free_start;

    total_start = alloc_start = rdtsc();
    for(int i = 0; i < test_count; i++)
    {
        test_array[i] = (test_struct*)xmem_alloc(pool);
    }
    alloc_total = rdtsc() - alloc_start;

    free_start = rdtsc();
    for(int i = 0; i < test_count; i++)
    {
        xmem_free(pool, (char*)test_array[i]);
    }
    free_total = rdtsc() - free_start;
    xmem_destroy_pool(pool);

    total = rdtsc() - total_start;

    printf("=== perf of xmem\n");
    printf("alloc & free times: %d\n", test_count);
    printf("alloc time: %llu CPU cycles\n", alloc_total);
    printf("free time: %llu CPU cycles\n", free_total);
    printf("total time: %llu CPU cycles\n", total);

    return 0;
}

