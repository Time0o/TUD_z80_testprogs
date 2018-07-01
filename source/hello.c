int main()
{
#ifdef Z80
  __asm
    LD BC, #0x1234
    HALT
  __endasm;
#endif

  return 0;
}
