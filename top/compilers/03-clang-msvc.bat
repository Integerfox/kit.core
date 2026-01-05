@echo off
:: Set environment for using Clang with MSVC backend

:: Get the version from the compiler itself
set _CC_VER=
for /f "tokens=3 delims= " %%a in ('clang --version 2^>nul ^| findstr /R /C:"^clang version"') do set _CC_VER=%%a

:: Set the name for the toolchain. It includes the compiler version
set _NAME=Clang/MSVC-backend compiler
if not "%_CC_VER%"=="" (
    set NAME=%_NAME%. Ver:%_CC_VER%
) else (
    set NAME=%_NAME%
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
    :: Updated PATH for Clang (MSVC)
    call set "PATH=%%PATH%%;C:\compilers\clang-msvc-x86_64\bin"

    :: Debugging is done via the MSVC backend, aka 'devenv.exe'
    doskey vsdbg="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /debugexe $*
)

:: Display the version info
for /f "tokens=3 delims= " %%a in ('clang --version 2^>nul ^| findstr /R /C:"^clang version"') do set _CC_VER=%%a
echo:Environment set: %_NAME%. Ver:%_CC_VER%
