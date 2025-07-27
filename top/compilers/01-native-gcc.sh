#!/bin/bash
# Set environment for using the native GCC compiler that is in the command path

if [ "$1" == "name" ]; then
    echo "Native GCC compiler"

else
	# Ensure environment path gets cleared
	unset NQBP_GCC_BIN

	echo "Environment set for native GCC compiler"
fi

