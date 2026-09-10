#!/bin/bash
# This is helper script that launches Segger's Ozone debugger
# The script assumes that Ozone is in command path
#
# usage: debug.sh [ozonefile]

OFILE="ozone.jdebug"
if [ "$1" != "" ]; then
    OFILE="$1"
fi

ozone -project "$OFILE" &
