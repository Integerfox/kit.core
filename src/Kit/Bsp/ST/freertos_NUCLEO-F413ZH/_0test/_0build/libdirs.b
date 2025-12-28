# KIT Middle-ware
src/Kit/Container
src/Kit/Text
src/Kit/Io

# KIT OSAL
src/Kit/System
src/Kit/System/_fatalerror < null.cpp
src/Kit/System/FreeRTOS
src/Kit/System/_assert
src/Kit/System/_trace

# Platform
src/Kit/Text/_mappings/_arm_gcc_stm32

# BSP
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH 
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX 
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Src > freertos.c

# SEGGER SysVIEW
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH/SeggerSysView

# SDK
xpkgs/stm32f4-sdk/Drivers/STM32F4xx_HAL_Driver/Src > stm32f4xx_hal_timebase_rtc_alarm_template.c stm32f4xx_hal_timebase_rtc_wakeup_template.c stm32f4xx_hal_timebase_tim_template.c

# FreeRTOS (With SEGGER SYSVIEW integration)
xpkgs/freertos-v10
xpkgs/freertos-v10/portable/MemMang < heap_1.c
xpkgs/freertos-v10/portable/GCC/ARM_CM4F

