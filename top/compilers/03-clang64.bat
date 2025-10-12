@echo off
:: Updated PATH for Clang (MSVC)
set _CLANG_CC_=\compilers\clang-msvc-x86_64\bin
IF "/%NQBP2_DONOT_ADD_CLANG_TO_PATH%" == "/" set PATH=%_CLANG_CC_%;%PATH% & set NQBP2_DONOT_ADD_CLANG_TO_PATH=true

:: Get the version from the compiler itself
for /f "tokens=3 delims= " %%a in ('clang --version ^| findstr /R /C:"^clang version"') do set _CC_VER=%%a

:: Display the version info
echo:Clang Compiler Environment set. Ver=%_CC_VER%, 64bit only
