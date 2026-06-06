::@echo off
REM GDB wrapper script to setup environment
call %~dp0env.bat 3
set DEBUGINFOD_URLS=
gdb.exe %*
