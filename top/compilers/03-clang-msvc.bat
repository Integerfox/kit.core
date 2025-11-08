@echo off
:: Updated PATH for Clang (MSVC)
set _CC_PATH_=\compilers\clang-msvc-x86_64\bin
IF "/%NQBP2_DONOT_ADD_CLANG_TO_PATH%" == "/" set PATH=%_CC_PATH_%;%PATH% & set NQBP2_DONOT_ADD_CLANG_TO_PATH=true

:: Get the version from the compiler itself
set _CC_VER=
for /f "tokens=3 delims= " %%a in ('clang --version ^| findstr /R /C:"^clang version"') do set _CC_VER=%%a

:: Debugging is done via the MSVC backend, aka 'devenv.exe'
doskey vsdbg="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /debugexe $*

:: Display the version info
echo:Clang Compiler Environment set. Ver=%_CC_VER% (msvc backend)
