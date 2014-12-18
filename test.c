/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  Testing code for xmempool.
 *
 *        Version:  1.0
 *        Created:  2014/12/09 11时53分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  XadillaX
 *   Organization:  Huaban.com
 *
 * =====================================================================================
 */
#include "xmempool.h"
#include <stdio.h>
#include <stdlib.h>

#define TEST_COUNT 4096

typedef struct test_struct {
    int  id;
    char str[16];
} test_struct;

void generate_str(char* str)
{
    str[15] = 0;
    for(int i = 0; i < 15; i++)
    {
        str[i] = (rand() % 26) + 'a';
    }
}

void print_test_struct(test_struct* test)
{
    printf("%d [0x%8X]: %s\n", test->id, (int)test, test->str);
}

int main()
{
    xmem_pool_handle pool = xmem_create_pool(sizeof(test_struct));
    test_struct* test_array[TEST_COUNT];
    for(int i = 0; i < TEST_COUNT; i++)
    {
        test_array[i]     = (test_struct*)xmem_alloc(pool);
        test_array[i]->id = i;
        generate_str(test_array[i]->str);
        print_test_struct(test_array[i]);
    }

    for(int i = 0; i < TEST_COUNT / 2; i++)
    {
        xmem_free(pool, (char*)test_array[i]);
    }

    xmem_print_info(pool);
    printf("\n");

    for(int i = TEST_COUNT / 2; i < TEST_COUNT; i++)
    {
        xmem_free(pool, (char*)test_array[i]);
    }

    xmem_print_info(pool);

    xmem_destroy_pool(pool);

    return 0;
}

