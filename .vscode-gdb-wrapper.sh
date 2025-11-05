#!/bin/bash
# GDB wrapper script to setup environment
source "$(dirname "$0")/env.sh" 2
gdb "$@"
