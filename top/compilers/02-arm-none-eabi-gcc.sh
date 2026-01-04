#!/bin/bash
# Set environment for using the ARM7 M/R Cortex Cross compiler that is in the command path

# Set the name for the toolchain.  It includes the compiler version
GCC_VERSION=$(arm-none-eabi-gcc --version 2>/dev/null | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -n1)
NAME="ARM7 M/R Cortex Cross compiler"
if [ -n "$GCC_VERSION" ]; then
    NAME="$NAME (version $GCC_VERSION)"
else
    NAME="$NAME (not found)"
fi

# Respond to name query
if [ "$1" == "name" ]; then
    echo $"$NAME"

# Respond to isConfigured query
elif [ "$1" == "isConfigured" ]; then
    if [ -n "$GCC_VERSION" ]; then
        echo $NAME
    else
        echo ""
    fi

# Set/Configure the toolchain
elif [ -n "$GCC_VERSION" ]; then
    echo "Environment set: $NAME"

# Compiler not found/not installed
else
    echo "Error: The $NAME needs to be in the PATH"
fi
