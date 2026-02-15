#!/bin/bash
# This is helper script that launches the RPi's picotool load utility for
# flashing the built binary onto the connected Pico2W device.  ASSUMES
# the picotool is in the command path.
#
# Default: is to load the first (non bs2_default.elf) .elf file found in the _pico directory
# usage: load.sh [elffile]

EFILE=""
if [ -n "$1" ]; then
    EFILE="$1"
else
    for f in _pico/*.elf; do
        if [ -f "$f" ] && [[ ! "$f" =~ bs2_default\.elf ]]; then
            EFILE="$f"
            break
        fi
    done
    
    if [ -z "$EFILE" ]; then
        echo "No .elf file found in _pico directory"
        exit 1
    fi
fi

echo "Loading   $EFILE   onto Pico2W device..."
picotool load -fx "$EFILE"
