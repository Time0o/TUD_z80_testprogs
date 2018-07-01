#include <stdint.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

void swap(int16_t *a, int16_t *b);
void swaparr(int16_t *a, int16_t *b, uint8_t len);
int16_t crc(int16_t *in, int16_t intermediate, int8_t len);

int main()
{
  int16_t a = 0;
  int16_t b = 0x4512;

  int16_t ar[5] = {0, 0, 0, 0, 0};
  int16_t br[5] = {0x1f43, 0x3243, 0x6421, 0x7715, 0x6321};

  int16_t endresult = 0;

  swap(&a, &b);
  swaparr(ar, br, 5);

  endresult = crc(&a, endresult, 1);
  endresult = crc(ar, endresult, 5);

#ifndef NDEBUG
  printf("result: 0x%04hx\n", endresult);
#endif

#ifdef Z80
  __asm
    LD B,H
    LD C,L
  __endasm;
#endif

  return 0;
}

void swap(int16_t *a, int16_t *b)
{
  int16_t tmp = *a;
  *a = *b;
  *b = tmp;
}

void swaparr(int16_t *a, int16_t *b, uint8_t len)
{
  for (uint8_t i = 0; i < len; ++i)
    swap(a + i, b + i);
}

int16_t crc(int16_t *in, int16_t intermediate, int8_t len)
{
  int16_t result = intermediate;
  for (int8_t i = 0; i < len; ++i)
    result ^= in[i];

  return result;
}
