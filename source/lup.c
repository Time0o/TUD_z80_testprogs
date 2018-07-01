#include <stdint.h>

#ifndef NDEBUG
#include <stdio.h>

static void debugmat(int16_t *mat, int16_t *pi, uint8_t rows, uint8_t cols)
{
  printf("pi: [");
  for (uint8_t i = 0; i < rows; ++i)
    printf(i == rows - 1 ? "%d]\n" : "%d, ", pi[i]);

  printf("A: [\n");
  for (uint8_t i = 0; i < rows; ++i) {
    printf(" [");
    for (uint8_t j = 0; j < cols; ++j) {
      int16_t val = mat[i * cols + j];
      double fval = (double) val / 256.0;

      printf(j == cols - 1 ? "0x%04hx (%.3f)]" : "0x%04hx (%.3f), ", val, fval);
    }
    printf(i == rows - 1 ? "\n" : ",\n");
  }
  printf("]\n");
}
#endif

int16_t fixpoint(int16_t i);
uint16_t absval(int16_t i);
int16_t mul(int16_t a, int16_t b, int8_t fixpoint);
int16_t div(int16_t a, int16_t b);
void swap(int16_t *a, int16_t *b);
void swaparr(int16_t *a, int16_t *b, uint8_t len);
void lup(int16_t *mat, int16_t *pi, uint8_t rows, uint8_t cols);
int16_t crc(int16_t *in, int16_t intermediate, int8_t len);

int16_t main()
{
  uint8_t rows = 3;
  uint8_t cols = 3;

  int16_t mat[9] = {-1, -2, 2, -8, 2, 4, -5, -5, -6};
  int16_t pi[3] = {0, 0, 0};
  int16_t endresult = 0;

  for (int i = 0; i < 9; ++i)
    mat[i] = fixpoint(mat[i]);

  lup(mat, pi, rows, cols);

#ifndef NDEBUG
  debugmat(mat, pi, rows, cols);
#endif

  endresult = crc(pi, endresult, rows);
  endresult = crc(mat, endresult, mul(rows, cols, 0));

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

int16_t fixpoint(int16_t i)
{
  return i << 8;
}

uint16_t absval(int16_t i)
{
  return (uint16_t) (i < 0 ? -i : i);
}

int16_t mul(int16_t a, int16_t b, int8_t fixpoint)
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
  if (fixpoint)
    result >>= 8;

  ret = (int16_t) (result & (0xffff));
  return ((a & 0x8000) ^ (b & 0x8000)) ? -result : result;
}

int16_t div(int16_t a, int16_t b)
{
  uint32_t au = absval(a) << 8;
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
  return ((a & 0x8000) ^ (b & 0x8000)) ? -result : result;
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

void lup(int16_t *mat, int16_t *pi, uint8_t rows, uint8_t cols)
{
  int16_t idx1, idx2, idx3;
  uint8_t p;
  int16_t piv_max;
  int16_t tmp;

  for (uint8_t i = 0; i < rows; ++i)
    pi[i] = i + 1;

  for (uint8_t i = 0; i < rows - 1; ++i) {
    p = i;

    idx1 = mul(i, cols, 0) + i;
    piv_max = absval(mat[idx1]);

    for (uint8_t j = i + 1; j < rows; ++j) {
      idx1 = mul(j, cols, 0) + i;
      tmp = absval(mat[idx1]);

      if (tmp > piv_max) {
        p = j;
        piv_max = tmp;
      }
    }

    swap(pi + i, pi + p);

    idx1 = mul(i, cols, 0);
    idx2 = mul(p, cols, 0);
    swaparr(&mat[idx1], &mat[idx2], cols);

    for (uint8_t k = i + 1; k < rows; ++k) {
      idx1 = mul(k, cols, 0) + i;
      idx2 = mul(i, cols, 0) + i;
      mat[idx1] = div(mat[idx1], mat[idx2]);

      for (uint8_t l = i + 1; l < rows; ++l) {
        idx2 = mul(k, cols, 0) + l;
        idx3 = mul(i, cols, 0) + l;
        mat[idx2] -= mul(mat[idx1], mat[idx3], 1);
      }
    }
  }
}

int16_t crc(int16_t *in, int16_t intermediate, int8_t len)
{
  int16_t result = intermediate;
  for (int8_t i = 0; i < len; ++i)
    result ^= in[i];

  return result;
}
