@echo off
:: This is helper script that launches the RPi's picotool load utility for
:: flashing the built binary onto the connected Pico2W device.  ASSUMES
:: the picotool is in the command path.
::
:: Default: is to load the first (non bs2_default.elf) .elf file found in the _pico directory
:: usage: load.bat [elffile]

set EFILE=
IF NOT "/%1"=="/%1" SET EFILE=%1
IF "%EFILE%"=="" (
    for %%f in (_pico\*.elf) do (
        echo %%f | findstr /i /v "bs2_default.elf" >nul
        if not errorlevel 1 (
            set EFILE=%%f
            goto :foundfile
        )
    )
    echo No .elf file found in _pico directory
    goto :eof
)
:foundfile
echo Loading   %EFILE%   onto Pico2W device...
picotool load -fx %EFILE% 

:eof
