/** @refer to http://www.mcs.anl.gov/~kazutomo/rdtsc.h */
#ifndef __RDTSC_H_DEFINED__
#define __RDTSC_H_DEFINED__
#include <stdint.h>

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define INLINE inline
#else
#define ALWAYS_INLINE __forceinline
#define INLINE __inline
#endif

#if defined(__i386__)

ALWAYS_INLINE uint64_t rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
#elif defined(__x86_64__)

ALWAYS_INLINE uint64_t rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#elif defined(__powerpc__)

ALWAYS_INLINE uint64_t rdtsc(void)
{
  unsigned long long int result=0;
  unsigned long int upper, lower,tmp;
  __asm__ volatile(
                "0:                  \n"
                "\tmftbu   %0           \n"
                "\tmftb    %1           \n"
                "\tmftbu   %2           \n"
                "\tcmpw    %2,%0        \n"
                "\tbne     0b         \n"
                : "=r"(upper),"=r"(lower),"=r"(tmp)
                );
  result = upper;
  result = result<<32;
  result = result|lower;

  return(result);
}

#elif defined(__aarch64__)

ALWAYS_INLINE uint64_t rdtsc(void)
{
    uint64_t virtual_timer_value;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
    return virtual_timer_value;
}

#else

#error "No tick counter is available!"

#endif


/*  $RCSfile:  $   $Author: kazutomo $
 *  $Revision: 1.6 $  $Date: 2005/04/13 18:49:58 $
 */

#endif


