#!/bin/bash
# This script is used to display compiler options and/or set the compiler environment
#
# NOTE: This script MUST be run in the top/compiler directory!!!!
#
# Usage: compiler-list [<n>]

# Get the list of available compilers (writes list to a temp file)
rm -f _compiler_.txt
index=0

for i in ./compilers/*.sh; do
    COMPILER=$("$i" name)
    ((index+=1))
    echo "$index,$i,$COMPILER" >> _compiler_.txt
done
if [ ! -f _compiler_.txt ]; then
    echo "No compilers have been configured/are available."
    echo "Check the contents of the top/compilers directory."

else
    # Display list of configured compilers
    if [ "$1" == "isConfigured" ]; then
        found=0
        while IFS=, read -r index filename compiler; do
            CONFIGURED=$("$filename" isConfigured)
            if [ -n "$CONFIGURED" ]; then
                echo "--> $CONFIGURED"
                ((found+=1))
            fi
        done < _compiler_.txt
        if [ $found -eq 0 ]; then
            echo "(none)"
        fi

    # Display list of all available compilers
    elif [ -z "$1" ]; then
        awk -F, '{print $1, "-", $3}' _compiler_.txt
        echo ""
    else

        # Configure compilers
        NQBP_CC_SELECTED="no compiler selected"
        while IFS=, read -r index filename compiler; do
            if [ "$index" == "$1" ]; then
                NQBP_CC_SELECTED="$compiler"
                source ./"$filename"
                echo
                break
            fi
        done < _compiler_.txt
    fi
fi