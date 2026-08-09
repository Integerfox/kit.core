@echo off
:: Set environment for using Visual Studio VC17 (32bit) compiler for Windows

:: Get the version from the compiler itself
set _CC_VER=
for /f "tokens=7 delims= " %%a in ('cl 2^>^&1 ^| findstr /R /C:"Version 19\..* for x86$"') do set _CC_VER=%%a

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
call :resolve_vcvars
if ERRORLEVEL 1 exit /b 1
call "%_VCVARS_BAT%"

:: Display the version info
for /f "tokens=7 delims= " %%a in ('cl 2^>^&1 ^| findstr /R /C:"Version"') do set _CC_VER=%%a
echo:Environment set: Visual Studio VC17 x86. Ver:%_CC_VER%
exit /b 0

:resolve_vcvars
set _VCVARS_BAT=
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find VC\Auxiliary\Build\vcvars32.bat 2^>nul`) do set _VCVARS_BAT=%%i
)

if not "%_VCVARS_BAT%"=="" goto :eof

@REM if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" set _VCVARS_BAT=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat
@REM if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat" set _VCVARS_BAT=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat
@REM if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat" set _VCVARS_BAT=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat
@REM if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat" set _VCVARS_BAT=C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat

if "%_VCVARS_BAT%"=="" (
    echo:ERROR: Unable to locate vcvars32.bat for Visual Studio 2022.
    exit /b 1
)
goto :eof

