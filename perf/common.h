#ifndef PERF_COMMON_H_
#define PERF_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "rdtsc.h"

#define MAX_TEST_COUNT 10000000
int test_count = 0;

typedef struct test_struct {
  int id;
  char str[16];
} test_struct;

test_struct* test_array[MAX_TEST_COUNT + 1];

void deal_with_test_count(int argc, char** argv) {
  if (argc <= 1) {
    test_count = 1000000;
    return;
  }

  char* str_times = argv[1];
  int i_times = atoi(str_times);

  if (i_times < 0) i_times = 0;
  if (i_times > MAX_TEST_COUNT) i_times = MAX_TEST_COUNT;

  test_count = i_times;
}

#endif  // PERF_COMMON_H_
