/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  perf common.
 *
 *        Version:  1.0
 *        Created:  2014/12/15 17时29分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  XadillaX
 *   Organization:  Huaban.com
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define TEST_COUNT 1000000

static inline uint64_t rdtsc(void)
{
    union {
        uint64_t tsc_64;
        struct {
            uint32_t lo_32;
            uint32_t hi_32;
        };
    } tsc;
 
    __asm volatile("rdtsc" :
        "=a" (tsc.lo_32),
        "=d" (tsc.hi_32));
    return tsc.tsc_64;
}

typedef struct test_struct {
    int id;
    char str[16];
} test_struct;

