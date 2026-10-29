@echo off
chcp 65001 >nul
echo ========================================
echo  STM32H742VITx - pyOCD RTT (log to file)
echo ========================================
echo.

set LOGFILE=pyocd_rtt_%DATE:~0,4%%DATE:~5,2%%DATE:~8,2%_%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%.log
set LOGFILE=%LOGFILE: =0%

C:\Users\31466\AppData\Roaming\Python\Python313\Scripts\pyocd.exe rtt ^
  -t stm32h742vitx ^
  -f 4000000 ^
  -a 0x20000000 -s 0x20000 ^
  -d "%LOGFILE%"
