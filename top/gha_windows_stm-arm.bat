:: This script is used by the CI/Build machine to build using the GCC  
:: ARM Cortex M/R compiler on a Windows host
::
:: usage: gha_windows_gcc-arm-mcu.bat <buildNumber>
::
set _TOPDIR=%~dp0
set _ROOT=%_TOPDIR%..
echo:%_ROOT%
echo:%_TOPDIR%

::
:: Build STM projects
::
:: Set up the compiler (which is included in the repo)
call ./env.bat 5
echo:%PATH%

:: Build the projects
cd %_ROOT%\projects
python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows stm-arm -c --bldtime --bld-all --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

:: Build the unit tests
cd %_ROOT%\src
python %NQBP_BIN%\other\bob.py -v4 --script-prefix python --p2 windows stm-arm -c --bldtime --bld-all --bldnum %BUILD_NUMBER%
IF ERRORLEVEL 1 EXIT /b 1

::
:: Everything worked!
::
exit /b 0
