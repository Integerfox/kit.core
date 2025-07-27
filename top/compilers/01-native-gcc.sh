#!/bin/bash
# Set environment for using the native GCC compiler that is in the command path

if [ "$1" == "name" ]; then
    echo "Native GCC compiler"

else
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

	echo "Environment set for native GCC compiler"
fi

