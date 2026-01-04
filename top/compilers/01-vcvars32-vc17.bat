@echo off
:: Set environment for using Visual Studio VC17 (32bit) compiler for Windows

:: Get the version from the compiler itself
set _CC_VER=
for /f "tokens=7 delims= " %%a in ('cl 2^>^&1 ^| findstr /R /C:"for x86$"') do set _CC_VER=%%a

:: Set the name for the toolchain. It includes the compiler version
set NAME=Visual Studio VC17 x86
IF NOT "/%_CC_VER%"=="/" SET NAME=%NAME%. Ver:%_CC_VER%

:: Respond to name query
if "%1"=="name" (
    echo:%NAME%
    exit /b 0
)

:: Respond to isConfigured query
if "%1"=="isConfigured" (
    if not "/%_CC_VER%"=="/" (
        echo:%NAME%
    )
    exit /b 0
)

:: Set/Configure the toolchain
if "/%_CC_VER%"=="/" (
    :: Run Visual Studio VC17 (32bit) environment setup
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
)
:: Display the version info
for /f "tokens=7 delims= " %%a in ('cl 2^>^&1 ^| findstr /R /C:"Version"') do set _CC_VER=%%a
SET NAME=%NAME%. Ver:%_CC_VER%
echo:Environment set: %NAME%

