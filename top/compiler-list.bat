@echo off
:: This script is used to display compiler options and/or set the compiler environment
::
:: NOTE: This script MUST be run in the top/compiler directory!!!!
::
:: usage: compiler-list [<n>]


:: Get the list of available compilers
call compiler-list-helper
IF NOT EXIST _compiler_.txt GOTO :notselected

:: Display list of configured compilers
IF "%1"=="isConfigured" GOTO :showConfigured

:: Display list of compilers
IF "/%1"=="/" GOTO :displaylist

:: Configure compilers
set NQBP_CC_SELECTED=no compiler selected
for /f "tokens=1,2,3* delims=," %%i in (_compiler_.txt) do (
    IF "/%%i"=="/%1" SET NQBP_CC_SELECTED=%%k & call compilers\%%j
)
IF "%NQBP_CC_SELECTED%"=="no compiler selected" GOTO :invalidselection
exit /b 0

:: No available compilers
:notselected
echo:No compilers have been configured/are available.
echo:Check the contents of the top/compilers directory.
exit /b 1

:: Invalid selection
:invalidselection
echo:Invalid compiler selection.  
echo:Try running env.bat without arguments for the list of available compilers.
exit /b 1

:: Show configured compilers
:showConfigured
setlocal enabledelayedexpansion
set found=0
for /f "tokens=1,2,3* delims=," %%i in (_compiler_.txt) do (
    for /f "delims=" %%x in ('compilers\%%j isConfigured') do (
        if not "%%x"=="" (
            echo:--^> %%x
            set /a found+=1
        )
    )
)
if !found! equ 0 (
    echo:^(none^)
)
endlocal
exit /b 0

:: Dynamically discovered list
:displaylist
echo:Available compiler toolchains:
for /f "tokens=1,2,3* delims=," %%i in (_compiler_.txt) do echo:%%i - %%k
exit /b 0