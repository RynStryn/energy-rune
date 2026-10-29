@echo off
chcp 65001 >nul
echo ========================================
echo  STM32H742VITx - pyOCD RTT Console
echo ========================================
echo.

C:\Users\31466\AppData\Roaming\Python\Python313\Scripts\pyocd.exe rtt ^
  -t stm32h742vitx ^
  -f 4000000 ^
  -a 0x20000000 -s 0x20000
