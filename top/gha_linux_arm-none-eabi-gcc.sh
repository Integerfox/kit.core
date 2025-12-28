#!/usr/bin/env bash
set -e
set -x

# This script is used by the CI/Build machine to build the Linux projects
#
# The script ASSUMES that the working directory is the package root
#
# usage: gha_linux_stm-arm.sh <buildNumber>
# 

# setup the environment
source ./env.sh 2

# Get script directory and set paths
_TOPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
_ROOT="$_TOPDIR/.."
BUILD_NUMBER="$1"

echo "Root:          $_ROOT"
echo "Top:           $_TOPDIR"
echo "Build Number:  $BUILD_NUMBER"

# Set the CI build flag
export NQBP_CI_BUILD="1"

# Build the projects
pushd projects
$NQBP_BIN/other/bob.py -v4 --p2 linux gcc-arm-mcu -c --bldtime --bld-all --bldnum "$BUILD_NUMBER"
popd

# Build the unit tests
pushd src
$NQBP_BIN/other/bob.py -v4 --p2 linux gcc-arm-mcu -c --bldtime --bld-all --bldnum "$BUILD_NUMBER"
popd
