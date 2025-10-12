:: This script is used by the CI\Build machine to build using the Clang  
:: compiler for the Windows 64BIT projects
::
:: usage: gha_windows_clang-host <buildNumber> 
::
set _TOPDIR=%~dp0
set _ROOT=%_TOPDIR%..
set BUILD_NUMBER=%1
echo:Root          %_ROOT%
echo:Top           %_TOPDIR%
echo:Build Number  %BUILD_NUMBER%

:: Set the CI build flag
set NQBP_CI_BUILD=1

:: Set the NQBP_BIN path (and other magic variables - but no compiler selected)
:: The Clang compiler already exists on the build machine and is the command path
call .\env.bat

::
:: Build Clang projects
::

::echo on
::call %_ROOT%\env.bat 3

:: Build the Catch2 static library
cd %_ROOT%\projects
::python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows clang-host -c --try win64
python %NQBP_BIN%\other\bob.py -v --script-prefix python --p2 windows clang-host -c --try win64


:: Build the unit tests
cd %_ROOT%\src
::python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows clang-host -c --bldtime --try win64 --bldnum %BUILD_NUMBER%
python %NQBP_BIN%\other\bob.py -v --script-prefix python --p2 windows clang-host -c --bldtime --try win64 --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

:: Run unit tests
cd %_ROOT%\src
python %NQBP_BIN%\other\chuck.py -vt --match a.exe --d2 windows --dir clang-host
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -v --match aa.exe --d2 windows --dir clang-host
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -vt --script-prefix python --match a.py --d2 windows --dir clang-host
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -v --script-prefix python --match aa.py --d2 windows --dir clang-host
IF ERRORLEVEL 1 EXIT /b 1

::
:: Everything worked!
::
exit /b 0
