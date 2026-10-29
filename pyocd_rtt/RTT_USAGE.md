# SEGGER RTT 使用手册 (DAPLink + pyOCD 版)

## 什么是 RTT？

RTT（Real Time Transfer）是 SEGGER 公司提出的一种调试输出方案，用 SWD 接口的两根线（SWDIO + SWCLK）**实时**传输数据，**不占用**串口、USB 等硬件资源。

对比：

| 方案 | 需要额外硬件 | 速度 | 影响程序实时性 |
|------|-------------|------|--------------|
| 串口 printf | UART 引脚 + USB-TTL | 115200~2M bps | 无 |
| **RTT** | **无**（用调试器即可）| **~1 MB/s** | 几乎无（只写内存）|
| SWO | 需要 SWO 引脚 | ~1 MB/s | 无 |

**RTT 不需要额外接线**，调试器的 SWDIO/SWCLK 已经在板上了。

---

## 1. 源码结构

```
pyocd_rtt/rtt/
├── SEGGER_RTT.h              # API 头文件
├── SEGGER_RTT_Conf.h         # 配置文件（缓冲区大小、通道数）
├── SEGGER_RTT.c              # 核心实现
├── RTT_Syscalls_GCC.c        # [可选] 重定向 printf 到 RTT（GCC 工具链）
└── RTT_Syscalls_KEIL.c       # [可选] 重定向 printf 到 RTT（Keil MDK）
```

---

## 2. 集成到工程

### 2.1 最小集成（只加两个文件）

```
SEGGER_RTT.c
SEGGER_RTT.h
SEGGER_RTT_Conf.h
```

调用 API 输出：

```c
#include "SEGGER_RTT.h"

int main(void) {
    // RTT 初始化（默认编译时已初始化，可省略）
    SEGGER_RTT_Init();

    // ---- 3 种输出方式 ----
    // 方式 1: 直接写字符串
    SEGGER_RTT_WriteString(0, "Hello RTT!\n");

    // 方式 2: 写二进制数据
    uint32_t val = 0xDEADBEEF;
    SEGGER_RTT_Write(0, &val, sizeof(val));

    // 方式 3: printf 格式输出（需要 CONFIG_SEGGER_RTT_PRINTF=1）
    SEGGER_RTT_printf(0, "Counter = %d, Temp = %.2f°C\n", counter, temp);

    // ---- 接收数据（主机→目标）----
    char buf[64];
    int n = SEGGER_RTT_Read(0, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        // 处理收到的命令
    }
}
```

### 2.2 完全接管 printf（加 Syscalls 文件）

| 工具链 | 添加的文件 | 效果 |
|--------|-----------|------|
| **GCC (arm-none-eabi)** | `RTT_Syscalls_GCC.c` | `printf()`, `puts()` → RTT |
| **Keil MDK (ARMCC)** | `RTT_Syscalls_KEIL.c` | `printf()` → RTT |

加了这个文件之后，**原有 printf 代码不用改**，自动输出到 RTT。

---

## 3. 配置详解

编辑 `SEGGER_RTT_Conf.h`：

```c
// 上行通道数（目标→主机，用于输出）
#define CONFIG_SEGGER_RTT_MAX_NUM_UP_BUFFERS     (3)

// 下行通道数（主机→目标，用于输入）
#define CONFIG_SEGGER_RTT_MAX_NUM_DOWN_BUFFERS   (3)

// 通道 0 缓冲区大小（默认上行 1KB，下行 16 字节）
#define BUFFER_SIZE_UP          (1024)    // 输出缓冲区，越大越不容易丢
#define BUFFER_SIZE_DOWN        (16)      // 输入缓冲区，一般不用太大

// 缓冲区满时的行为
// 0 = 跳过（丢弃新数据，不阻塞）
// 1 = 修剪（覆盖最旧的数据，环形缓冲区）
// 2 = 阻塞（等待，直到有空间——会影响程序实时性）
#define CONFIG_SEGGER_RTT_DEFAULT_MODE    (0)
```

**建议**：`BUFFER_SIZE_UP` 设大一点（2KB~4KB），保证频繁输出时不会丢数据。

---

## 4. 多通道用法

RTT 支持多个独立通道，适用于区分不同模块的输出：

```c
// 通道 0 — 系统消息
SEGGER_RTT_WriteString(0, "System booting...\n");

// 通道 1 — 调试信息（使用自定义缓冲区）
static char dbg_buf[2048];
SEGGER_RTT_ConfigUpBuffer(1, "Debug", dbg_buf, sizeof(dbg_buf),
                          SEGGER_RTT_MODE_DEFAULT);
SEGGER_RTT_WriteString(1, "ADC value: ...\n");

// 通道 2 — 错误日志
static char err_buf[512];
SEGGER_RTT_ConfigUpBuffer(2, "Error", err_buf, sizeof(err_buf),
                          SEGGER_RTT_MODE_NO_BLOCK_TRIM);
```

pyOCD 默认只显示通道 0，需要用 `--up-channel-id` 或 `--down-channel-id` 切换：

```bash
# 看通道 1 的输出
pyocd rtt -t stm32f405rgtx --up-channel-id 1
```

---

## 5. 终端色彩

RTT 通道 0 支持 6 个虚拟终端，用色彩区分：

```c
// 红色 = 错误
SEGGER_RTT_SetTerminal(0);   // 默认终端
SEGGER_RTT_TerminalOut(1, "[ERROR] Something broke!\n");  // 红色
SEGGER_RTT_TerminalOut(2, "[WARN]  Battery low\n");       // 黄色
SEGGER_RTT_TerminalOut(3, "[INFO]  Service started\n");   // 绿色
SEGGER_RTT_TerminalOut(4, "[DEBUG] Var = 42\n");          // 蓝色
```

在 pyOCD RTT 终端中会按颜色显示。

---

## 6. 接收输入（主机→目标）

RTT 不仅仅是输出，也可以**从主机键盘输入**发送到目标：

```c
char cmd[64];
int len = SEGGER_RTT_Read(0, cmd, sizeof(cmd) - 1);
if (len > 0) {
    cmd[len] = '\0';
    // 解析命令...
}
```

在 pyOCD RTT 终端中，**键盘输入会自动发送到目标**的下行通道。

---

## 7. RTOS 环境

### FreeRTOS
`SEGGER_RTT_Conf.h` 中修改锁宏：

```c
#include "FreeRTOS.h"
#include "task.h"
#define SEGGER_RTT_LOCK()    taskENTER_CRITICAL()
#define SEGGER_RTT_UNLOCK()  taskEXIT_CRITICAL()
```

### RT-Thread / 其他 RTOS
替换为对应关中断或互斥锁的宏即可。

---

## 8. 多核注意事项 (STM32H7)

STM32H742 是单核 Cortex-M7，直接使用即可。

如果是双核型号（H745/H747），注意：
- RTT Control Block 必须放在**两个核都能访问的内存区域**
- 推荐放到 D2 domain（AXI SRAM，0x24000000）
- 用链接脚本将 RTT Control Block 固定到 DTCM 或 AXI SRAM

```c
// 在 SEGGER_RTT_Conf.h 中指定段
#define SEGGER_RTT_CB_SECTION  ".ram_dtcm"   // 放入 DTCM
```

---

## 9. 完整工作流程

```
┌─────────────────┐     SWD      ┌──────────────┐     USB      ┌────────────┐
│  目标板         │◄────────────►│  DAPLink      │◄───────────►│  Windows   │
│  STM32F405RG    │              │  CMSIS-DAP    │             │            │
│  ┌───────────┐  │              │              │             │ pyocd rtt  │
│  │ RTT 缓冲区 │  │              │              │             │  终端显示  │
│  │ 0x20000000│  │              │              │             │            │
│  └───────────┘  │              │              │             │ 按 Ctrl+C  │
│  SEGGER_RTT.c   │              │              │             │  停止      │
└─────────────────┘              └──────────────┘             └────────────┘
```

1. 固件编译，包含 RTT 源码
2. 烧录到目标板
3. 目标板上电运行
4. Windows 执行 `pyocd_rtt\rtt_f405rg.bat`
5. pyOCD 通过 DAPLink 扫描 RAM，找到 RTT Control Block
6. 实时显示输出，键盘输入实时发送

---

## 10. 常见问题

**Q: pyOCD 说 "RTT control block not found"**

A: 可能原因：
- 目标板没运行（复位/供电检查）
- RTT 缓冲区在扫描范围外 → 增大 `-a` / `-s` 范围
- 目标板有 MPU 保护调试器读 RAM → 检查 MPU 配置
- 链接脚本优化把 `_SEGGER_RTT` 优化掉了 → `__attribute__((used))` 确认

**Q: 输出不完整 / 丢数据**

A: 增大 `BUFFER_SIZE_UP`（`SEGGER_RTT_Conf.h` 中改），目前默认是 1KB。

**Q: 怎么用 J-Link 的 RTT Viewer 替代 pyOCD？**

A: 固件代码完全一样，J-Link RTT Viewer 也能自动搜索 Control Block。换调试器不影响代码。

**Q: printf 重定向后只能输出到 RTT，串口没了？**

A: 是的，`_write()` 被替换了。如果需要同时输出到串口，保留 RTT 调用，不要加 Syscalls 文件，手动混合使用 `SEGGER_RTT_WriteString` 和 `UART_Send`。

---
