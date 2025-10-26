echo off
IF "/%1"=="/name" ECHO:GCC-ARM (none-eabi) compiler for STM32 & exit /b 0

:: Segger support
set PATH=%PATH%;c:\Program Files (x86)\SEGGER\JLink
set PATH=%PATH%;c:\Program Files\SEGGER\Ozone

set PATH=C:\compilers\gcc-arm-none-eabi\bin;%PATH%
echo:GCC Arm-none-eabi Environment set to xsrc\stm32-gcc-arm\bin