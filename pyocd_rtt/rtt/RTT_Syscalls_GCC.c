/*********************************************************************
*               SEGGER RTT Syscalls — GCC (newlib / newlib-nano)
*
*  Overrides _write() so printf() / puts() output goes to RTT channel 0.
*
*  Usage: add this file to your project. No other changes needed.
*********************************************************************/

#include <reent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "SEGGER_RTT.h"

__attribute__((weak))
int _write(int file, char* ptr, int len) {
  (void)file;
  SEGGER_RTT_Write(0, (const void*)ptr, (unsigned)len);
  return len;
}

__attribute__((weak))
int _read(int file, char* ptr, int len) {
  (void)file;
  return (int)SEGGER_RTT_Read(0, ptr, (unsigned)len);
}

__attribute__((weak))
int _fstat(int file, struct stat* st) {
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

__attribute__((weak))
int _isatty(int file) {
  (void)file;
  return 1;
}

__attribute__((weak))
int _lseek(int file, int ptr, int dir) {
  (void)file; (void)ptr; (void)dir;
  return 0;
}

__attribute__((weak))
int _close(int file) {
  (void)file;
  return -1;
}
