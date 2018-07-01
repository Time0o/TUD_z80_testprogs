#include <stdint.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

uint16_t absval(int16_t i);
int16_t mul(int16_t a, int16_t b);

int main()
{
  int16_t a = -0x0213;
  int16_t b = 0x0542;
  int16_t c = 0x1111;

  int16_t tmp = mul(a, b);
  int16_t result = mul(tmp, c);

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

uint16_t absval(int16_t i)
{
  return (uint16_t) (i < 0 ? -i : i);
}

int16_t mul(int16_t a, int16_t b)
{
  uint32_t au = absval(a);
  uint32_t bu = absval(b);
  uint32_t result = 0;
  int16_t ret;

  while (bu > 0) {
    if (bu & 1)
      result += au;
    au <<= 1;
    bu >>= 1;
  }
  result >>= 8;

  ret = (int16_t) (result & (0xffff));
  return ((a & 0x8000) ^ (b & 0x8000)) ? -ret : ret;
}
