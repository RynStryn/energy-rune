/*********************************************************************
*               SEGGER RTT Syscalls — ARM Compiler (Keil MDK)
*
*  Overrides fputc() / _sys_write() so printf() goes to RTT channel 0.
*********************************************************************/

#include "SEGGER_RTT.h"
#include <stdio.h>

#if defined(__ARMCC_VERSION) || defined(__CC_ARM)

int fputc(int ch, FILE* f) {
  (void)f;
  char c = (char)ch;
  SEGGER_RTT_Write(0, &c, 1);
  return ch;
}

int fgetc(FILE* f) {
  (void)f;
  char c;
  if (SEGGER_RTT_Read(0, &c, 1) == 0) {
    return -1;
  }
  return (int)c;
}

int _sys_write(int fh, const unsigned char* buf, unsigned len, int mode) {
  (void)fh; (void)mode;
  SEGGER_RTT_Write(0, (const void*)buf, len);
  return 0; // 0 = success
}

// Keil: implement _ttywrch for putchar()
void _ttywrch(int ch) {
  char c = (char)ch;
  SEGGER_RTT_Write(0, &c, 1);
}

#endif
