# Hardware test sources
src/Kit/Driver/NV/Flash/_0test/_hw_flash

# Platform-specific main
src/Kit/Driver/NV/Flash/_0test/_hw_flash/_0build/freertos_NUCLEO-F413ZH_W25Q

# NV interface
src/Kit/Driver/NV

# NV Flash driver (template headers, no source files to compile)
#  - Api.h / Api_.h are template headers included directly by the test

# W25Q flash driver
src/Kit/Driver/Flash/W25Q

# Checksum (CRC32 for NV flash)
src/Kit/Checksum

# SPI HAL driver
src/Kit/Driver/SPI/ST/M32F4

# DIO HAL driver
src/Kit/Driver/Dio/ST/M32F4


# Output stream for the KIT Console
src/Kit/Io/Serial/ST/M32F4

# KIT Middle-ware
src/Kit/Container
src/Kit/Text
src/Kit/Io
src/Kit/EventQueue
src/Kit/Itc

# KIT OSAL
src/Kit/System
src/Kit/System/FreeRTOS
src/Kit/System/FreeRTOS/_fatalerror
src/Kit/System/_assert
src/Kit/System/_trace


# BSP
src/Kit/Text/_mappings/_arm_gcc_stm32
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH 
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX 
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Src > freertos.c

# SEGGER SysVIEW
src/Kit/Bsp/ST/freertos_NUCLEO-F413ZH/SeggerSysView

# SDK
xpkgs/stm32f4-sdk/Drivers/STM32F4xx_HAL_Driver/Src > stm32f4xx_hal_timebase_rtc_alarm_template.c stm32f4xx_hal_timebase_rtc_wakeup_template.c stm32f4xx_hal_timebase_tim_template.c

# FreeRTOS
xpkgs/freertos-v10
xpkgs/freertos-v10/portable/MemMang < heap_1.c
xpkgs/freertos-v10/portable/GCC/ARM_CM4F
