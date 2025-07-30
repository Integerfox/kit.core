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

# Generate code coverage metrics 
#RANDOM_UNIT_TEST_DIR="$NQBP_PKG_ROOT/tests/Cpl/Checksum/_0test/linux/gcc"
#COMBINED_CODE_COVERAGE_FILE="$NQBP_PKG_ROOT/cobertura.json"
#COMBINED_CODE_COVERAGE_XML="$NQBP_PKG_ROOT/cobertura.xml"
#if [ -f "$COMBINED_CODE_COVERAGE_FILE" ]; then
#    rm -f "$COMBINED_CODE_COVERAGE_FILE"
#fi
#pushd src
#$NQBP_BIN/other/chuck.py -v --d2 linux --dir gcc-host --match tca.py args --ci rpt --json cobertura.json
#$NQBP_BIN/other/chuck.py -v --d2 linux --dir gcc-host --match tca.py args --ci merge cobertura.json $COMBINED_CODE_COVERAGE_FILE
#popd

# Convert the JSON data file to XML format (need to use the tca.py script to get the correct gcov args)
#pushd $RANDOM_UNIT_TEST_DIR
#python tca.py --ci rpt -a $COMBINED_CODE_COVERAGE_FILE --xml $COMBINED_CODE_COVERAGE_XML
#popd