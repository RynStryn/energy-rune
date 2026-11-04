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

#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H

#ifdef __IAR_SYSTEMS_ICC__
  #include <intrinsics.h>
#endif

/*********************************************************************
*       Defines, configurable
*********************************************************************/

//
// Maximum number of up (target->host) and down (host->target) RTT buffers
// Up to 16 channels are supported in total.
//
#ifndef CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS
  #define CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS    (3)    // Default: 3 up channels
#endif

#ifndef CONFIG_SEGGER_RTT_MAX_NUM_DOWN_BUFFERS
  #define CONFIG_SEGGER_RTT_MAX_NUM_DOWN_BUFFERS  (3)    // Default: 3 down channels
#endif

//
// Size of the default up (target->host) and down (host->target) buffers
//
#ifndef BUFFER_SIZE_UP
  #define BUFFER_SIZE_UP          (1024)           // 1 KB for each default up buffer
#endif

#ifndef BUFFER_SIZE_DOWN
  #define BUFFER_SIZE_DOWN        (16)             // 16 bytes for each default down buffer
#endif

//
// Write mode: how to handle writes when buffer is full
// 0: Skip (drop new data) — No blocking, no trimming
// 1: Trim  — overwrite oldest data (FIFO-like)
// 2: Block — spin-wait until space becomes available
//
#ifndef CONFIG_SEGGER_RTT_DEFAULT_MODE
  #define CONFIG_SEGGER_RTT_DEFAULT_MODE          (0)
#endif

//
// Circular buffer handling: when buffer wraps, copy in two parts or one?
// 0: Use memcpy for whole copy (simpler). 1: Handle wrap manually (faster large buffers)
//
#ifndef CONFIG_SEGGER_RTT_WRITE_USE_MEMMOVE
  #define CONFIG_SEGGER_RTT_WRITE_USE_MEMMOVE     (0)
#endif

//
// Printf buffer size for SEGGER_RTT_printf()
//
#ifndef CONFIG_SEGGER_RTT_PRINTF_BUFFER_SIZE
  #define CONFIG_SEGGER_RTT_PRINTF_BUFFER_SIZE    (64)
#endif

//
// Printf support: 0 = no SEGGER_RTT_printf (saves code size)
//
#ifndef CONFIG_SEGGER_RTT_PRINTF
  #define CONFIG_SEGGER_RTT_PRINTF                (1)
#endif

//
// Lock / Unlock macros for task-safety (used in RTOS environments)
// Default: disable interrupts (Cortex-M: __disable_irq / __enable_irq)
//
// For FreeRTOS, you could define these to use taskENTER_CRITICAL()/taskEXIT_CRITICAL()
//
#ifndef SEGGER_RTT_LOCK
  #if defined(__ICCARM__)
    #define SEGGER_RTT_LOCK()    { __disable_interrupt(); __disable_irq(); }
    #define SEGGER_RTT_UNLOCK()  { __enable_irq(); __enable_interrupt(); }
  #elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
    #define SEGGER_RTT_LOCK()    { __disable_irq(); }
    #define SEGGER_RTT_UNLOCK()  { __enable_irq(); }
  #else  /* GCC / Clang */
    static inline void _SEGGER_RTT_LOCK(void)   { __asm volatile ("cpsid i"); }
    static inline void _SEGGER_RTT_UNLOCK(void) { __asm volatile ("cpsie i"); }
    #define SEGGER_RTT_LOCK()    _SEGGER_RTT_LOCK()
    #define SEGGER_RTT_UNLOCK()  _SEGGER_RTT_UNLOCK()
  #endif
#endif

//
// Place RTT Control Block in a specific section (for linker control)
// Default: no special section (placed in .bss)
//
// #define SEGGER_RTT_CB_SECTION ".ram_dtcm"   // e.g., place in DTCM on STM32H7
//

#endif
