#!/usr/bin/env bash
# This script is used to set the compiler environment for a Linux host.
#
# NOTE: The scripts in the top/compilers directory MUST be customized for PC.
#       (only the compilers that you use need to be customized)
#
# Usage: env [<compiler>]
#set -x

HERE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# ensure the CWD is in the path
export PATH=$PATH:.

# Set the NQBP_BIN path
export NQBP_PKG_ROOT="$HERE"
export NQBP_WORK_ROOT="$HERE"/..
export NQBP_XPKGS_ROOT="$HERE"/xpkgs
export NQBP_BIN="$NQBP_XPKGS_ROOT/nqbp2"
export NQBP_SHELL_SCRIPT_EXTENSION=".sh"

# Add Ninja to the command path, but only once
if [ -z "$NQBP2_DONOT_ADD_NINJA_TO_PATH" ]; then
    export PATH=$PATH:$NQBP_BIN/ninja
    export NQBP2_DONOT_ADD_NINJA_TO_PATH=true
fi

# Outcast setup
export OUTCAST2_PATH="$NQBP_XPKGS_ROOT/outcast"
export PATH="$PATH:$OUTCAST2_PATH/bin"

# Support for the SINELABORE code generator
export SINELABORE_PATH=~/sinelabore/bin

# Set helper macros
alias t="cd $HERE"
alias bob="$NQBP_BIN/other/bob.py"
alias chuck="$NQBP_BIN/other/chuck.py"
alias ratt="$NQBP_XPKGS_ROOT/ratt/bin/ratt.py"
alias fixx="$NQBP_BIN/other/fixx.py"
alias whatcc="echo $NQBP_CC_SELECTED"
alias vcc="$NQBP_PKG_ROOT/top/tca2.py --html-dir $NQBP_PKG_ROOT/docs/publish/code-coverage view"
alias map="$NQBP_PKG_ROOT/scripts/kit.core/map.py" 
alias vdox="xdg-open $NQBP_PKG_ROOT/docs/publish/doxygen/index.html &>/dev/null &" 
alias bgcc="$NQBP_BIN/other/bob.py -v --p2 linux gcc-host"
alias tnat="$NQBP_BIN/other/chuck.py -v --dir linux --match a.out --m2 a.py --m3 aa.out --m4 aa.py"


# No compiler option selected
if [ -z "$1" ]; then
	pushd $HERE/top >/dev/null 2>&1
	echo "Current toolchain: $NQBP_CC_SELECTED"
	./compiler-list.sh
	popd >/dev/null 2>&1
else
    pushd $HERE/top >/dev/null 2>&1
	source ./compiler-list.sh $1
	EVARS=$(printenv | grep NQBP)
	IFS=$'\n'; for e in $EVARS; do export $e; done;
	popd >/dev/null 2>&1
fi
