cmake_minimum_required(VERSION 3.15)
project(Energy_Organs C ASM)

# =============================================
# 包含路径（代码跳转用）
# =============================================
include_directories(
    0lib_new
    0lib_old
    HEADFILE
    README
    STLIB/inc
    USER
    USER/RTE/_Template
)

# =============================================
# 宏定义
# =============================================
add_definitions(
    -DSTM32F40_41xxx
    -DSTM32F407xx
    -DUSE_STDPERIPH_DRIVER
)

# =============================================
# 全部源文件
# =============================================
add_executable(${PROJECT_NAME}
    CORE/startup_stm32f40_41xxx.s
    STLIB/src/misc.c
    STLIB/src/stm32f4xx_adc.c
    STLIB/src/stm32f4xx_can.c
    STLIB/src/stm32f4xx_crc.c
    STLIB/src/stm32f4xx_cryp.c
    STLIB/src/stm32f4xx_cryp_aes.c
    STLIB/src/stm32f4xx_cryp_des.c
    STLIB/src/stm32f4xx_cryp_tdes.c
    STLIB/src/stm32f4xx_dac.c
    STLIB/src/stm32f4xx_dbgmcu.c
    STLIB/src/stm32f4xx_dcmi.c
    STLIB/src/stm32f4xx_dma.c
    STLIB/src/stm32f4xx_dma2d.c
    STLIB/src/stm32f4xx_exti.c
    STLIB/src/stm32f4xx_flash.c
    STLIB/src/stm32f4xx_flash_ramfunc.c
    STLIB/src/stm32f4xx_fmc.c
    STLIB/src/stm32f4xx_fsmc.c
    STLIB/src/stm32f4xx_gpio.c
    STLIB/src/stm32f4xx_hash.c
    STLIB/src/stm32f4xx_hash_md5.c
    STLIB/src/stm32f4xx_hash_sha1.c
    STLIB/src/stm32f4xx_i2c.c
    STLIB/src/stm32f4xx_iwdg.c
    STLIB/src/stm32f4xx_ltdc.c
    STLIB/src/stm32f4xx_pwr.c
    STLIB/src/stm32f4xx_rcc.c
    STLIB/src/stm32f4xx_rng.c
    STLIB/src/stm32f4xx_rtc.c
    STLIB/src/stm32f4xx_sai.c
    STLIB/src/stm32f4xx_sdio.c
    STLIB/src/stm32f4xx_spi.c
    STLIB/src/stm32f4xx_syscfg.c
    STLIB/src/stm32f4xx_tim.c
    STLIB/src/stm32f4xx_usart.c
    STLIB/src/stm32f4xx_wwdg.c
    STLIB/src/system_stm32f4xx.c
    USER/main.c
    USER/stm32f4xx_it.c
    USER/system_stm32f4xx.c
    USER/BSP.c
    0lib_old/can.c
    0lib_old/CanBus.c
    0lib_old/control_task.c
    0lib_old/led.c
    0lib_old/RC.c
    0lib_old/senior.c
    0lib_old/sys.c
    0lib_old/timer.c
    0lib_old/usart.c
    0lib_old/WS2812B.c
    0lib_new/graphics.c
    0lib_new/led_panel.c
    0lib_new/map.c
    0lib_new/protocol.c
    0lib_new/water_flow.c
    0lib_new/ws2812.c
)
