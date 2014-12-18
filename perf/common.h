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
#include "rdtsc.h"

#define TEST_COUNT 1000000

typedef struct test_struct {
    int  id;
    char str[16];
} test_struct;

