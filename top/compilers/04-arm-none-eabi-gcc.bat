@echo off
:: Set environment for using the ARM7 M/R Cortex Cross compiler

:: Get the version from the compiler itself
set _CC_VER=
for /f "tokens=8 delims=) " %%a in ('arm-none-eabi-gcc --version 2^>nul ^| findstr /R /C:"^arm-none-eabi-gcc"') do set _CC_VER=%%a

:: Set the name for the toolchain. It includes the compiler version
set NAME=ARM7 M/R Cortex Cross compiler
if not "%_CC_VER%"=="" (
    set NAME=%NAME%. Ver:%_CC_VER%
) else (
    set NAME=%NAME%
)

:: Respond to name query
if "%1"=="name" (
    echo:%NAME%
    exit /b 0
)

:: Respond to isConfigured query
if "%1"=="isConfigured" (
    if not "%_CC_VER%"=="" (
        echo:%NAME%
    )
    exit /b 0
)

:: Set/Configure the toolchain
if "/%_CC_VER%"=="/" (
    echo:Configuring ARM7 M/R Cortex Cross compiler environment...
    :: Updated PATH for GCC ARM R/M Cross compiler
    call set "PATH=%%PATH%%;C:\compilers\gcc-arm-none-eabi\bin"

    :: Segger support
    call set "PATH=%%PATH%%;c:\Program Files (x86)\SEGGER\JLink"
    call set "PATH=%%PATH%%;c:\Program Files\SEGGER\Ozone"
)

:: Display the version info
for /f "tokens=8 delims=) " %%a in ('arm-none-eabi-gcc --version 2^>nul ^| findstr /R /C:"^arm-none-eabi-gcc"') do set _CC_VER=%%a
echo:Environment set: %NAME%. Ver:%_CC_VER%
 
