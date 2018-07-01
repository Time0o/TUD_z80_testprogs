#include <stdint.h>

int8_t add(int8_t a, int8_t b);

int main()
{
  int8_t a = 1;
  int8_t b = 2;
  int8_t c = 0;

  c = add(a, b);

#ifdef Z80
  __asm
    LD B, #0
    LD C, L
  __endasm;
#endif

  return 0;
}

int8_t add(int8_t a, int8_t b)
{
  return a + b;
}
