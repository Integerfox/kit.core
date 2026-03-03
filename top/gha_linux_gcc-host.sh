#!/usr/bin/env bash
set -e
set -x

# This script is used by the CI/Build machine to build the Linux projects
#
# The script ASSUMES that the working directory is the package root
#
# usage: build_linux_gcc-host.sh <buildNumber>
# 

# setup the environment
source ./env.sh 1

# NOTE: ONLY BUILDING 64Bit executables right now

# Set the CI build flag
export NQBP_CI_BUILD="1"

# Build the Catch2 static library FIRST 
pushd projects/xpkgs/catch2
$NQBP_BIN/other/bob.py -v4 --p2 linux gcc-host -c --try posix64 
popd

# Build all test linux projects 
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

# Build example projects (and any unit tests they may contain)
pushd projects
$NQBP_BIN/other/bob.py -v4 --p2 linux gcc-host -c --try posix64 

# Run all unit tests under the projects/ tree
$NQBP_BIN/other/chuck.py -vt --match a.out --d2 linux --dir gcc-host
$NQBP_BIN/other/chuck.py -v --match aa.out --d2 linux --dir gcc-host
$NQBP_BIN/other/chuck.py -vt --match a.py  --d2 linux --dir gcc-host
$NQBP_BIN/other/chuck.py -v --match aa.py  --d2 linux --dir gcc-host
popd


