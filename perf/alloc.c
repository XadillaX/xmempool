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

uint64_t total       = 0;
uint64_t alloc_total = 0;
uint64_t free_total  = 0;

int main(int argc, char** argv)
{
    deal_with_test_count(argc, argv);

    int size = sizeof(test_struct);
    uint64_t total_start, alloc_start, free_start;

    total_start = alloc_start = rdtsc();
    for(int i = 0; i < test_count; i++)
    {
        test_array[i] = (test_struct*)malloc(size);
    }
    alloc_total = rdtsc() - alloc_start;

    free_start = rdtsc();
    for(int i = 0; i < test_count; i++)
    {
        free(test_array[i]);
    }
    free_total = rdtsc() - free_start;
    total = rdtsc() - total_start;

    printf("=== perf of malloc\n");
    printf("alloc & free times: %d\n", test_count);
    printf("alloc time: %llu CPU cycles\n", alloc_total);
    printf("free time: %llu CPU cycles\n", free_total);
    printf("total time: %llu CPU cycles\n", total);

    return 0;
}

