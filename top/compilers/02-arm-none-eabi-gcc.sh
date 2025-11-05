#!/bin/bash
# Set environment for using the ARM7 M/R Cortex Cross compiler that is in the command path

if [ "$1" == "name" ]; then
    echo "ARM7 M/R Cortex Cross compiler"

else
    # Get compiler version
    GCC_VERSION=$(arm-none-eabi-gcc --version 2>/dev/null | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -n1)
    if [ -n "$GCC_VERSION" ]; then
        echo "Environment set for ARM7 M/R Cortex Cross compiler (version $GCC_VERSION)"
    else
        echo "Error: The path to the ARM7 M/R Cortex Cross compiler needs to be added to the PATH"
    fi
fi

