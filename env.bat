@echo off
:: This script is used to set the compiler environment for a Windows host.
::
:: NOTE: The scripts in the top/compilers directory MUST be customized for PC.
::       (only the compilers that you use need to be customized)
::
:: Usage: env [<compiler>]

set _HERE=%~dp0

:: ensure the CWD is in the path (only add if not already present)
echo ;%PATH%; | find ";.;" >nul
if ERRORLEVEL 1 set PATH=%PATH%;.

:: Set the NQBP_BIN path
set NQBP_PKG_ROOT=%_HERE%
set NQBP_WORK_ROOT=%_HERE%..\
set NQBP_XPKGS_ROOT=%_HERE%xpkgs
set NQBP_BIN=%NQBP_XPKGS_ROOT%\nqbp2
set NQBP_SHELL_SCRIPT_EXTENSION=.bat

:: Add Ninja to the command path if not already available
ninja --version >nul 2>nul
if ERRORLEVEL 1 set PATH=%PATH%;%NQBP_BIN%\ninja

:: Outcast setup
set OUTCAST2_PATH=%NQBP_XPKGS_ROOT%\outcast
set PATH=%OUTCAST2_PATH%\bin;%PATH%

:: Set helper macros
doskey top=cd %_HERE%
doskey t=cd %_HERE%
doskey sancho=%NQBP_BIN%\other\sancho.py $*
doskey bob=%NQBP_BIN%\other\bob.py $*
doskey chuck=%NQBP_BIN%\other\chuck.py $*
doskey ratt=%NQBP_XPKGS_ROOT%\ratt\bin\ratt.py $*
doskey fixx=%NQBP_BIN%\other\fixx.py $*
doskey map=%NQBP_PKG_ROOT%\scripts\kit\map.py $*
doskey genfsm2=%NQBP_BIN%\other\genfsm2.py $*
doskey bmsvc=%NQBP_BIN%\other\bob.py -v --p2 windows msvc $*
doskey bclang=%NQBP_BIN%\other\bob.py -v --p2 windows clang-host $*
doskey tnat=%NQBP_BIN%\other\chuck.py -v --dir windows --match a.exe --m2 a.py --m3 aa.exe --m4 aa.py $*
doskey killpy=taskkill /F /IM python.exe
doskey setexe=git update-index --chmod=+x $*
doskey ports=reg query HKLM\HARDWARE\DEVICEMAP\SERIALCOMM

:: No compiler option selected
IF "/%1"=="/" GOTO :displaycc

:: Set the compiler toolchain
pushd %_HERE%top
call compiler-list.bat %1
popd
exit /b 0

:: display compiler options
:displaycc
pushd %_HERE%top
echo:Configured toolchains:
call compiler-list.bat isConfigured
echo:
call compiler-list.bat
popd
exit /b 0
