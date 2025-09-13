#!/usr/bin/env bash
set -e
set -x

# This script is used by the CI/Build machine to build the Linux projects
#
# The script ASSUMES that the working directory is the package root
#
# usage: build_linux_gcc-host.sh <buildNumber> [branch]
# 

# setup the environment
source ./env.sh 1


# Set the CI build flag
export NQBP_CI_BUILD="1"

# Build the Catch2 static library (just 64bit apps for now)
pushd projects
$NQBP_BIN/other/bob.py -v4 --p2 linux gcc-host -c --try posix64 
popd

# Build all test linux projects (just 64bit apps for now)
pushd src
$NQBP_BIN/other/bob.py -v4 --p2 linux gcc-host -c --try posix64 --bldtime --bldnum $1

# Run unit tests
$NQBP_BIN/other/chuck.py -vt --match a.out --d2 linux --dir gcc-host
$NQBP_BIN/other/chuck.py -v --match aa.out --d2 linux --dir gcc-host
$NQBP_BIN/other/chuck.py -vt --match a.py  --d2 linux --dir gcc-host
$NQBP_BIN/other/chuck.py -v --match aa.py  --d2 linux --dir gcc-host
popd

# Generate code coverage metrics for the entire src/ tree
pushd src
$NQBP_PKG_ROOT/top/tca2.py --html-dir $NQBP_PKG_ROOT/docs/publish/code-coverage ci
popd
