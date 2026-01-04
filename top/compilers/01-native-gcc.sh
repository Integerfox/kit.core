#!/bin/bash
# Set environment for using the native GCC compiler that is in the command path

# Set the name for the toolchain.  It includes the compiler version
GCC_VERSION=$(gcc --version 2>/dev/null | head -n1 | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -n1)
NAME="Native GCC compiler"
if [ -n "$GCC_VERSION" ]; then
    NAME="$NAME (version $GCC_VERSION)"
else
    NAME="$NAME (not found)"
fi

# Respond to name query
if [ "$1" == "name" ]; then
    echo "$NAME"

# Respond to isConfigured query
elif [ "$1" == "isConfigured" ]; then
    if [ -n "$GCC_VERSION" ]; then
        echo $NAME
    else
        echo ""
    fi
    
# Set/Configure the toolchain
elif [ -n "$GCC_VERSION" ]; then
    # Ensure environment path gets cleared
    unset NQBP_GCC_BIN

    # Create GDB wrapper script for VS Code debugging
    cat > "$NQBP_PKG_ROOT/.vscode-gdb-wrapper.sh" << 'EOF'
#!/bin/bash
# GDB wrapper script to setup environment
source "$(dirname "$0")/env.sh" 1
gdb "$@"
EOF
    chmod +x "$NQBP_PKG_ROOT/.vscode-gdb-wrapper.sh"
    echo "Environment set: $NAME"

# Compiler not found/not installed
else
    echo "Error: The native GCC compiler needs to be in the PATH"
fi

