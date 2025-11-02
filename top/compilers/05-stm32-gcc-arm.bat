@echo off
IF "/%1"=="/name" ECHO:GCC-ARM (none-eabi) compiler for STM32 & exit /b 0

:: Updated PATH for GCC ARM (none-eabi)
set _GCC_ARM_CC_=C:\compilers\gcc-arm-none-eabi\bin
set _SEGGER_JLINK_=c:\Program Files (x86)\SEGGER\JLink
set _SEGGER_OZONE_=c:\Program Files\SEGGER\Ozone

IF "/%NQBP2_DONOT_ADD_GCC_ARM_TO_PATH%" == "/" set PATH=%_GCC_ARM_CC_%;%_SEGGER_JLINK_%;%_SEGGER_OZONE_%;%PATH% & set NQBP2_DONOT_ADD_GCC_ARM_TO_PATH=true

:: Get the version from the compiler itself
for /f "tokens=10" %%a in ('arm-none-eabi-gcc --version ^| findstr /R /C:"^arm-none-eabi-gcc"') do set _CC_VER=%%a

:: Display the version info
echo:GCC ARM none-eabi Compiler Environment set. Ver=%_CC_VER%