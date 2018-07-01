#include <stdint.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

uint16_t absval(int16_t i);
int16_t div(int16_t a, int16_t b);

int main()
{
  int16_t a = -0x2f43;
  int16_t b = 0x0351;
  int16_t c = 0x0111;

  int16_t tmp = div(a, b);
  int16_t result = div(tmp, c);

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

int16_t div(int16_t a, int16_t b)
{
  uint32_t au = ((uint32_t) absval(a)) << 8;
  uint32_t bu = absval(b);
  uint32_t result = 0;
  int16_t ret;

  uint32_t sub = bu;
  uint32_t pos = 1;

  if (a == b)
    return 1;

  if (bu > au)
    return 0;

  while (sub <= au) {
    sub <<= 1;
    pos <<= 1;
  }

  sub >>= 1;
  pos >>= 1;

  while (pos) {
    if (sub <= au) {
      au -= sub;
      result |= pos;
    }
    sub >>= 1;
    pos >>= 1;
  }

  ret = (int16_t) (result & (0xffff));
  return ((a & 0x8000) ^ (b & 0x8000)) ? -ret : ret;
}
