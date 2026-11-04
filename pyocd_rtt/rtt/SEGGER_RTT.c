/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
* (c) 1995 - 2024 SEGGER Microcontroller GmbH                        *
*                                                                    *
* www.segger.com     Support: support@segger.com                     *
*                                                                    *
**********************************************************************
*/

#include "SEGGER_RTT.h"
#include <string.h>
#include <stdarg.h>

/*********************************************************************
*       Static data
*********************************************************************/

// Default up-buffer memory (channel 0: terminal I/O)
static char _acUpBuffer0[BUFFER_SIZE_UP];
static char _acDownBuffer0[BUFFER_SIZE_DOWN];

// Default up-buffer 1,2
#if CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS > 1
static char _acUpBuffer1[BUFFER_SIZE_UP];
#endif
#if CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS > 2
static char _acUpBuffer2[BUFFER_SIZE_UP];
#endif

// Printf scratch buffer (used only when CONFIG_SEGGER_RTT_PRINTF == 1)
#if CONFIG_SEGGER_RTT_PRINTF
static char _acPrintfBuffer[CONFIG_SEGGER_RTT_PRINTF_BUFFER_SIZE];
#endif

/*********************************************************************
*       Global data
*********************************************************************/

SEGGER_RTT_CB _SEGGER_RTT __attribute__((used)) =
{
  .aID            = "SEGGER RTT",
  .MaxNumUpBuffers   = CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS,
  .MaxNumDownBuffers = CONFIG_SEGGER_RTT_MAX_NUM_DOWN_BUFFERS,
  .aUp = {
    { .sName = "Terminal", .pBuffer = _acUpBuffer0, .SizeOfBuffer = sizeof(_acUpBuffer0), .WrOff = 0, .RdOff = 0, .Flags = CONFIG_SEGGER_RTT_DEFAULT_MODE },
#if CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS > 1
    { .sName = "Chan1",    .pBuffer = _acUpBuffer1, .SizeOfBuffer = sizeof(_acUpBuffer1), .WrOff = 0, .RdOff = 0, .Flags = CONFIG_SEGGER_RTT_DEFAULT_MODE },
#endif
#if CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS > 2
    { .sName = "Chan2",    .pBuffer = _acUpBuffer2, .SizeOfBuffer = sizeof(_acUpBuffer2), .WrOff = 0, .RdOff = 0, .Flags = CONFIG_SEGGER_RTT_DEFAULT_MODE },
#endif
  },
  .aDown = {
    { .sName = "Terminal", .pBuffer = _acDownBuffer0, .SizeOfBuffer = sizeof(_acDownBuffer0), .WrOff = 0, .RdOff = 0, .Flags = CONFIG_SEGGER_RTT_DEFAULT_MODE },
  }
};

//
// Weak symbol so that a user-provided init function takes priority
// if they want to configure buffers after startup.
//
__attribute__((weak)) void SEGGER_RTT_Init(void) {
  // Already initialized via compile-time .bss zeroing and the
  // initializer above.  This is a no-op by default.
}

/*********************************************************************
*       Static functions
*********************************************************************/

//
// Copy data into/from circular buffer, respecting wrap-around.
//
static unsigned _DoWrite(SEGGER_RTT_BUFFER_UP* pBuf, const char* pSrc, unsigned NumBytes) {
  unsigned Avail;
  unsigned WrOff;
  unsigned Remain;
  char*    pDst;

  WrOff  = pBuf->WrOff;
  pDst   = pBuf->pBuffer;
  Avail  = pBuf->SizeOfBuffer - pBuf->WrOff;

  if (NumBytes <= Avail) {
    // Single fragment: fits from WrOff to end
    memcpy(pDst + WrOff, pSrc, NumBytes);
    pBuf->WrOff = WrOff + NumBytes;
    if (pBuf->WrOff >= pBuf->SizeOfBuffer) {
      pBuf->WrOff = 0; // wrapped exactly
    }
  } else {
    // Two fragments: fill to end, then wrap to beginning
    if (Avail > 0) {
      memcpy(pDst + WrOff, pSrc, Avail);
      pSrc += Avail;
      NumBytes -= Avail;
    }
    // Wrap and write remaining
    WrOff = pBuf->WrOff + Avail;
    if (WrOff >= pBuf->SizeOfBuffer) {
      WrOff -= pBuf->SizeOfBuffer;
    }
    Remain = pBuf->SizeOfBuffer - pBuf->RdOff; // can't overwrite last unread byte? Actually no, that's a design choice.
    if (NumBytes > Remain) {
      NumBytes = Remain;
    }
    memcpy(pDst + WrOff, pSrc, NumBytes);
    pBuf->WrOff = WrOff + NumBytes;
  }
  return NumBytes;
}

static unsigned _DoRead(SEGGER_RTT_BUFFER_DOWN* pBuf, char* pDst, unsigned NumBytes) {
  unsigned Avail;
  unsigned RdOff;
  unsigned Remain;

  RdOff  = pBuf->RdOff;
  Avail  = pBuf->WrOff - RdOff;
  if (RdOff > pBuf->WrOff) {
    Avail = (pBuf->SizeOfBuffer - RdOff) + pBuf->WrOff;
  }

  if (NumBytes > Avail) {
    NumBytes = Avail;
  }

  if (NumBytes == 0) {
    return 0;
  }

  Remain = pBuf->SizeOfBuffer - RdOff;
  if (NumBytes <= Remain) {
    memcpy(pDst, pBuf->pBuffer + RdOff, NumBytes);
    pBuf->RdOff = RdOff + NumBytes;
    if (pBuf->RdOff >= pBuf->SizeOfBuffer) {
      pBuf->RdOff = 0;
    }
  } else {
    memcpy(pDst, pBuf->pBuffer + RdOff, Remain);
    pDst += Remain;
    memcpy(pDst, pBuf->pBuffer, NumBytes - Remain);
    pBuf->RdOff = NumBytes - Remain;
  }
  return NumBytes;
}

/*********************************************************************
*       Public API functions
*********************************************************************/

//
// Write to the specified up-buffer (target -> host)
//
unsigned SEGGER_RTT_Write(unsigned BufferIndex, const void* pBuffer, unsigned BufferSize) {
  unsigned NumBytes;
  SEGGER_RTT_BUFFER_UP* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumUpBuffers) {
    return 0;
  }

  pBuf = &_SEGGER_RTT.aUp[BufferIndex];
  SEGGER_RTT_LOCK();
  NumBytes = _DoWrite(pBuf, (const char*)pBuffer, BufferSize);
  SEGGER_RTT_UNLOCK();
  return NumBytes;
}

//
// Write string (null-terminated) to the specified up-buffer
//
unsigned SEGGER_RTT_WriteString(unsigned BufferIndex, const char* s) {
  unsigned Len;

  if (s == NULL) {
    return 0;
  }
  Len = strlen(s);
  return SEGGER_RTT_Write(BufferIndex, s, Len);
}

//
// Write with overwrite (no check): always writes, oldest data may be lost
//
void SEGGER_RTT_WriteWithOverwriteNoCheck(unsigned BufferIndex, const void* pBuffer, unsigned BufferSize) {
  SEGGER_RTT_BUFFER_UP* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumUpBuffers) {
    return;
  }
  pBuf = &_SEGGER_RTT.aUp[BufferIndex];
  SEGGER_RTT_LOCK();
  _DoWrite(pBuf, (const char*)pBuffer, BufferSize);
  SEGGER_RTT_UNLOCK();
}

//
// Read from the specified down-buffer (host -> target)
//
unsigned SEGGER_RTT_Read(unsigned BufferIndex, void* pBuffer, unsigned BufferSize) {
  unsigned NumBytes;
  SEGGER_RTT_BUFFER_DOWN* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumDownBuffers) {
    return 0;
  }
  pBuf = &_SEGGER_RTT.aDown[BufferIndex];
  SEGGER_RTT_LOCK();
  NumBytes = _DoRead(pBuf, (char*)pBuffer, BufferSize);
  SEGGER_RTT_UNLOCK();
  return NumBytes;
}

unsigned SEGGER_RTT_ReadNoLock(unsigned BufferIndex, void* pData, unsigned BufferSize) {
  SEGGER_RTT_BUFFER_DOWN* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumDownBuffers) {
    return 0;
  }
  pBuf = &_SEGGER_RTT.aDown[BufferIndex];
  return _DoRead(pBuf, (char*)pData, BufferSize);
}

//
// How many bytes are currently in the up-buffer, waiting to be read?
//
unsigned SEGGER_RTT_GetBytesInBuffer(unsigned BufferIndex) {
  unsigned Avail;
  SEGGER_RTT_BUFFER_UP* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumUpBuffers) {
    return 0;
  }
  pBuf = &_SEGGER_RTT.aUp[BufferIndex];

  Avail = pBuf->WrOff - pBuf->RdOff;
  if (pBuf->RdOff > pBuf->WrOff) {
    Avail = (pBuf->SizeOfBuffer - pBuf->RdOff) + pBuf->WrOff;
  }
  return Avail;
}

//
// How much space is left in the up-buffer?
//
unsigned SEGGER_RTT_GetAvailWriteSpace(unsigned BufferIndex) {
  unsigned Avail;
  SEGGER_RTT_BUFFER_UP* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumUpBuffers) {
    return 0;
  }
  pBuf = &_SEGGER_RTT.aUp[BufferIndex];

  Avail = pBuf->RdOff - pBuf->WrOff - 1;
  if (pBuf->WrOff >= pBuf->RdOff) {
    Avail = (pBuf->SizeOfBuffer - pBuf->WrOff) + pBuf->RdOff - 1;
  }
  return Avail;
}

//
// Configure an up-buffer (or reconfigure it)
//
int SEGGER_RTT_ConfigUpBuffer(unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags) {
  SEGGER_RTT_BUFFER_UP* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumUpBuffers) {
    return -1;
  }
  pBuf = &_SEGGER_RTT.aUp[BufferIndex];

  SEGGER_RTT_LOCK();
  pBuf->sName        = sName;
  pBuf->pBuffer      = (char*)pBuffer;
  pBuf->SizeOfBuffer = BufferSize;
  pBuf->WrOff        = 0;
  pBuf->RdOff        = 0;
  pBuf->Flags        = Flags;
  SEGGER_RTT_UNLOCK();
  return 0;
}

//
// Configure a down-buffer (or reconfigure it)
//
int SEGGER_RTT_ConfigDownBuffer(unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags) {
  SEGGER_RTT_BUFFER_DOWN* pBuf;

  if (BufferIndex >= (unsigned)_SEGGER_RTT.MaxNumDownBuffers) {
    return -1;
  }
  pBuf = &_SEGGER_RTT.aDown[BufferIndex];

  SEGGER_RTT_LOCK();
  pBuf->sName        = sName;
  pBuf->pBuffer      = (char*)pBuffer;
  pBuf->SizeOfBuffer = BufferSize;
  pBuf->WrOff        = 0;
  pBuf->RdOff        = 0;
  pBuf->Flags        = Flags;
  SEGGER_RTT_UNLOCK();
  return 0;
}

//
// Terminal control: set active terminal (channel 0, VT100-like)
//
void SEGGER_RTT_SetTerminal(unsigned char TerminalId) {
  char ac[6];

  ac[0] = 0xFF;
  ac[1] = 0;
  ac[2] = 0;
  ac[3] = (char)TerminalId;
  ac[4] = 0;
  ac[5] = 0;
  SEGGER_RTT_Write(0, ac, 6);
}

//
// Terminal output: write to a specific terminal channel
//
unsigned SEGGER_RTT_TerminalOut(unsigned char TerminalId, const char* s) {
  SEGGER_RTT_SetTerminal(TerminalId);
  return SEGGER_RTT_WriteString(0, s);
}

//
// printf via RTT (if enabled)
//
#if CONFIG_SEGGER_RTT_PRINTF
int SEGGER_RTT_printf(unsigned BufferIndex, const char* sFormat, ...) {
  int NumBytes;
  va_list ParamList;

  va_start(ParamList, sFormat);
  NumBytes = vsnprintf(_acPrintfBuffer, sizeof(_acPrintfBuffer), sFormat, ParamList);
  va_end(ParamList);

  if (NumBytes < 0) {
    return 0;
  }
  if ((unsigned)NumBytes >= sizeof(_acPrintfBuffer)) {
    NumBytes = (int)(sizeof(_acPrintfBuffer) - 1);
  }
  return SEGGER_RTT_Write(BufferIndex, _acPrintfBuffer, (unsigned)NumBytes);
}
#endif

//
// Lock / Unlock (weak so user can override)
//
__attribute__((weak)) void SEGGER_RTT_Lock(void) {
  SEGGER_RTT_LOCK();
}

__attribute__((weak)) void SEGGER_RTT_Unlock(void) {
  SEGGER_RTT_UNLOCK();
}
