@echo on
:: This script is used by the CI\Build machine to build using the Visual Studio 
:: compiler for the Windows projects
::
:: usage: gha_windows_msvc.bat <buildNumber>
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
call ./env.bat


::
:: Build Visual Studio projects
::

:: Build the Catch2 static library
cd %_ROOT%\projects
echo:Build Catch2 static library...
::python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows msvc -c --bld-all
python %NQBP_BIN%\other\bob.py -v --script-prefix python --p2 windows msvc -c --bld-all
IF ERRORLEVEL 1 EXIT /b 1

:: Build the unit tests
cd %_ROOT%\src
echo:Building unit tests...
::python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows msvc -c --bldtime --bld-all --bldnum %BUILD_NUMBER%
python %NQBP_BIN%\other\bob.py -v --script-prefix python --p2 windows msvc -c --bldtime --bld-all --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

:: Run unit tests
echo:Running unit tests...
cd %_ROOT%\src
python %NQBP_BIN%\other\chuck.py -vt --match a.exe --d2 windows --dir msvc
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -v --match aa.exe --d2 windows --dir msvc
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -vt --script-prefix python --match a.py --d2 windows --dir msvc
IF ERRORLEVEL 1 EXIT /b 1
python %NQBP_BIN%\other\chuck.py -v --script-prefix python --match aa.py --d2 windows --dir msvc
IF ERRORLEVEL 1 EXIT /b 1

::
:: Everything worked!
::
exit /b 0
