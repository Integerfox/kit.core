@echo off
:: Updated PATH for GCC ARM R/M Cross compiler
set _CC_PATH_=\compilers\gcc-arm-none-eabi\bin
IF "/%NQBP2_DONOT_ADD_GCC_ARM_TO_PATH%" == "/" set PATH=%_CC_PATH_%;%PATH% & set NQBP2_DONOT_ADD_GCC_ARM_TO_PATH=true

:: Get the version from the compiler itself
set _CC_VER=
for /f "tokens=8 delims=) " %%a in ('arm-none-eabi-gcc --version 2^>nul ^| findstr /R /C:"^arm-none-eabi-gcc"') do set _CC_VER=%%a

:: Segger support
set PATH=%PATH%;c:\Program Files (x86)\SEGGER\JLink
set PATH=%PATH%;c:\Program Files\SEGGER\Ozone

:: Display the version info
echo:GCC Arm-none-eabi Environment set. Ver=%_CC_VER% 
