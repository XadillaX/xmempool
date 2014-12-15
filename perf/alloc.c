/*
 * =====================================================================================
 *
 *       Filename:  alloc.c
 *
 *    Description:  alloc of native;
 *
 *        Version:  1.0
 *        Created:  2014/12/15 17时29分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  XadillaX
 *   Organization:  Huaban.com
 *
 * =====================================================================================
 */
#include "common.h"

test_struct*        test_array[TEST_COUNT];

int                 total = 0;
int                 alloc_total = 0;
int                 free_total = 0;

int main()
{
    int size = sizeof(test_struct);
    int total_start, alloc_start, free_start;

    total_start = alloc_start = rdtsc();
    for(int i = 0; i < TEST_COUNT; i++)
    {
        test_array[i] = (test_struct*)malloc(size);
    }
    alloc_total = rdtsc() - alloc_start;

    free_start = rdtsc();
    for(int i = 0; i < TEST_COUNT; i++)
    {
        free(test_array[i]);
    }
    free_total = rdtsc() - free_start;
    total = rdtsc() - total_start;

    printf("=== perf of malloc\n");
    printf("alloc & free times: %d\n", TEST_COUNT);
    printf("alloc time: %d CPU cycles\n", alloc_total);
    printf("free time: %d CPU cycles\n", free_total);
    printf("total time: %d CPU cycles\n", total);

    return 0;
}

