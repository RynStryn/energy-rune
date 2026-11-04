# pyOCD RTT 配置 — DAPLink/CMSIS-DAP

## 前置

```powershell
pip install pyocd
pyocd pack install stm32f405 stm32h742
```

## 使用方法

连接 DAPLink 调试器到目标板，然后双击对应的 bat 文件：

| 目标 | 命令 |
|---|---|
| **STM32F405RG** | `rtt_f405rg.bat` |
| **STM32H742VI** | `rtt_h742vi.bat` |
| **F405RG + 日志文件** | `rtt_f405rg_log.bat` |
| **H742VI + 日志文件** | `rtt_h742vi_log.bat` |

## 参数说明

| 参数 | 值 | 说明 |
|---|---|---|
| `-t` | 目标名称 | 芯片型号，pack 安装后自动识别 |
| `-f` | 4 MHz | SWD 时钟频率（可调更低提高稳定性） |
| `-a` | 0x20000000 | RTT Control Block 搜索起始地址 |
| `-s` | 0x20000 (F4) / DTCM | 搜索范围（128KB） |
| `-d` | 文件名 | 输出日志到文件（不指定则输出到终端） |

## RTT 扫描机制

pyOCD 自动在指定地址范围内搜索 SEGGER RTT Control Block 签名
`"SEGGER RTT"`（16字节对齐）。如果 RTT 区域不在默认位置，调整 `-a` 和 `-s`。

## 连接其他探头

pyOCD 支持 CMSIS-DAP v1/v2、ST-Link、J-Link。用 `-u <ID>` 指定
具体探头（`pyocd list --probes` 查看）。
