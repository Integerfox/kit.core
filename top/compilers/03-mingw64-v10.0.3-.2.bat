@echo off
IF "/%1"=="/name" ECHO:MINGW64 v10.3.0-2 compiler for Windows (32bit and 64 bit) & exit /b 0

:: Configuration...
set PATH=C:\TDM-GCC-64\bin;%PATH%

:: Create GDB wrapper script for VS Code debugging
(
echo ::@echo off
echo REM GDB wrapper script to setup environment
echo call %%~dp0env.bat 3
echo gdb.exe %%*
) > %NQBP_PKG_ROOT%\.vscode-gdb-wrapper.bat



echo:MinGW-x64 Compiler Environment set (v10.3.0-2 32bit and 64bit)
