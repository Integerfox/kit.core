:: This script is used by the CI\Build machine to build using the MinGW  
:: compiler for the Windows 64BIT projects
::
:: usage: gha_windows_gcc-host <buildNumber> 
::
set _TOPDIR=%~dp0
set _ROOT=%_TOPDIR%..
echo:%_ROOT%
echo:%_TOPDIR%

:: Set the CI build flag
set NQBP_CI_BUILD=1

:: Set the NQBP_BIN path (and other magic variables - but no compiler selected)
call ./env.bat

::
:: Build Mingw projects
::

::echo on
::call %_ROOT%\env.bat 3

:: Build the Catch2 static library
cd %_ROOT%\projects
python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows gcc-host -c --try win64


:: Build the unit tests
cd %_ROOT%\src
python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows gcc-host -c --bldtime --try win64 --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

:: Run unit tests
cd %_ROOT%\src
python %NQBP_BIN%\other\chuck.py -vt --match a.exe --d2 windows --dir gcc-host
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -v --match aa.exe --d2 windows --dir gcc-host
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -vt --script-prefix python --match a.py --d2 windows --dir gcc-host
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -v --script-prefix python --match aa.py --d2 windows --dir gcc-host
IF ERRORLEVEL 1 EXIT /b 1

::
:: Everything worked!
::
exit /b 0
