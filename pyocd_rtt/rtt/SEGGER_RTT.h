/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2024 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER RTT * Real Time Transfer for embedded targets         *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the RTT protocol and J-Link.                       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller GmbH BE       *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,             *
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED    *
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND    *
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT           *
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING    *
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF    *
* THE POSSIBILITY OF SUCH DAMAGE.                                   *
*                                                                    *
**********************************************************************
---------------------------END-OF-HEADER------------------------------
File    : SEGGER_RTT.h
Purpose : Implementation of SEGGER Real-Time Transfer (RTT) which
          allows real-time communication on targets which support
          debugger memory accesses while the CPU is running.
Revision: $Rev: 25842 $
**********************************************************************
*/

#ifndef SEGGER_RTT_H
#define SEGGER_RTT_H

#include "SEGGER_RTT_Conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
*       Defines, fixed
*********************************************************************/

#define SEGGER_RTT_MAX_NUM_UP_BUFFERS       (CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS)
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS     (CONFIG_SEGGER_RTT_MAX_NUM_DOWN_BUFFERS)

#define BUFFER_INDEX_UP                     0
#define BUFFER_INDEX_DOWN                   0

#define SEGGER_RTT_MODE_DEFAULT             (SEGGER_RTT_MODE_NO_BLOCK_SKIP)

//
// Target is a little-endian machine
//
#define TARGET_IS_LITTLE_ENDIAN             (1)

//
// Byte order defaults
//
#ifndef USE_DEBUG_GET_BUFFER_SIZE
  #define USE_DEBUG_GET_BUFFER_SIZE         (0)
#endif

//
// Wait mode: block / skip / trim
//
#define SEGGER_RTT_MODE_NO_BLOCK_SKIP       (0)
#define SEGGER_RTT_MODE_NO_BLOCK_TRIM       (1)
#define SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL  (2)

/*********************************************************************
*       Types
*********************************************************************/

//
// Description for a circular buffer (also used for up and down channels)
//
typedef struct {
  const    char*    sName;           // Channel name
           char*    pBuffer;         // Pointer to start of buffer
    unsigned        SizeOfBuffer;    // Buffer size in bytes (available bytes)
    unsigned        WrOff;           // Position of next item to be written
    unsigned        RdOff;           // Position of next item to be read
    unsigned        Flags;           // Configuration flags
} SEGGER_RTT_BUFFER_UP;

typedef struct {
  const    char*    sName;           // Channel name
           char*    pBuffer;         // Pointer to start of buffer
    unsigned        SizeOfBuffer;    // Buffer size in bytes (available bytes)
    unsigned        WrOff;           // Position of next item to be written
    unsigned        RdOff;           // Position of next item to be read
    unsigned        Flags;           // Configuration flags
} SEGGER_RTT_BUFFER_DOWN;

//
// RTT Control Block - describes the number of buffers, their sizes, etc.
// This is the data structure whose address MUST be known for the debugger
// to find the RTT data. It is identified by the aID signature.
//
typedef struct {
  char                    aID[16];                        // Initialized to "SEGGER RTT"
  int                     MaxNumUpBuffers;                // Max number of up-buffers (target->host)
  int                     MaxNumDownBuffers;              // Max number of down-buffers (host->target)
  SEGGER_RTT_BUFFER_UP    aUp[SEGGER_RTT_MAX_NUM_UP_BUFFERS];   // Up-buffers
  SEGGER_RTT_BUFFER_DOWN  aDown[SEGGER_RTT_MAX_NUM_DOWN_BUFFERS]; // Down-buffers
} SEGGER_RTT_CB;

/*********************************************************************
*       Global data
*********************************************************************/

extern SEGGER_RTT_CB _SEGGER_RTT;

/*********************************************************************
*       RTT API functions
*********************************************************************/

//
// Initialization
//
void SEGGER_RTT_Init(void);

//
// Write to RTT buffer (up channel, target -> host)
//
unsigned SEGGER_RTT_Write            (unsigned BufferIndex, const void* pBuffer, unsigned BufferSize);
unsigned SEGGER_RTT_WriteString      (unsigned BufferIndex, const char* s);
void     SEGGER_RTT_WriteWithOverwriteNoCheck(unsigned BufferIndex, const void* pBuffer, unsigned BufferSize);

//
// Read from RTT buffer (down channel, host -> target)
//
unsigned SEGGER_RTT_Read             (unsigned BufferIndex, void* pBuffer, unsigned BufferSize);
unsigned SEGGER_RTT_ReadNoLock       (unsigned BufferIndex, void* pData,   unsigned BufferSize);

//
// Query buffer state
//
unsigned SEGGER_RTT_GetBytesInBuffer (unsigned BufferIndex);
unsigned SEGGER_RTT_GetAvailWriteSpace(unsigned BufferIndex);

//
// Configure RTT buffer
//
int      SEGGER_RTT_ConfigUpBuffer   (unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags);
int      SEGGER_RTT_ConfigDownBuffer (unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags);

//
// Terminal output (channel 0, terminal control via \e[0..6m)
//
void     SEGGER_RTT_SetTerminal       (unsigned char TerminalId);
unsigned SEGGER_RTT_TerminalOut       (unsigned char TerminalId, const char* s);

//
// printf-style formatting via RTT
//
int      SEGGER_RTT_printf            (unsigned BufferIndex, const char* sFormat, ...);

//
// Lock / Unlock (for task-safety in RTOS) — user redefinable via SEGGER_RTT_LOCK/UNLOCK
//
void SEGGER_RTT_Lock(void);
void SEGGER_RTT_Unlock(void);

#ifdef __cplusplus
}
#endif

#endif
