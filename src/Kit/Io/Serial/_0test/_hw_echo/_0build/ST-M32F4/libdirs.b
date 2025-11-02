# Application Specific OSAL
#src/Kit/System/FreeRTOS/_fatalerror
#src/Kit/System/_trace
#src/Kit/Io/Stdio/_ansi

# Output stream for the trace engine (and the debug console)
#src/Kit/Io/Serial/ST/M32F4


# CPL Middle-ware
src/Kit/Container
src/Kit/Text
#src/Kit/Io/Stdio
#src/Kit/Io

# CPL OSAL
src/Kit/System
src/Kit/System/FreeRTOS
src/Kit/System/FreeRTOS/_fatalerror
src/Kit/System/_assert


# BSP
src/Kit/Text/_mappings/_arm_gcc_stm32
#src/Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/trace
src/Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH 
src/Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/MX 
src/Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/MX/Core/Src > freertos.c
#src/Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/console

# SEGGER SysVIEW
src/Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/SeggerSysView

# SDK
xpkgs/stm32F4-SDK/Drivers/STM32F4xx_HAL_Driver/Src > stm32f4xx_hal_timebase_rtc_alarm_template.c stm32f4xx_hal_timebase_rtc_wakeup_template.c stm32f4xx_hal_timebase_tim_template.c

# FreeRTOS
xpkgs/freertos-v10
xpkgs/freertos-v10/portable/MemMang < heap_1.c
xpkgs/freertos-v10/portable/GCC/ARM_CM4F

