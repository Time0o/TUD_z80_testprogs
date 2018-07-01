#include <stdint.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

int16_t crc(int16_t *in, int16_t intermediate, int8_t len);

volatile int16_t result = 0x0;

int16_t main()
{
  int16_t arr[5] = {0x1643, 0x6212, 0x2521, 0x1643, 0x3261};
  result = crc(arr, 0, 5);

#ifndef NDEBUG
  printf("result: 0x%04hx\n", result);
#endif

#ifdef Z80
  __asm
    LD B,H
    LD C,L
  __endasm;
#endif

  return 0;
}

int16_t crc(int16_t *in, int16_t intermediate, int8_t len)
{
  int16_t result = intermediate;
  for (int8_t i = 0; i < len; ++i)
    result ^= in[i];

  return result;
}
