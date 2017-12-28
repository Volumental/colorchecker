#!/bin/bash
set -eu

# Safe way to find location of the script, using $0 does not
# work when script is sourced. First the path to the script
# is retrieved from BASH_SOURCE. After changing to this dir
# (resolves relative paths, etc.) pwd gives the absolute path
# to the script dir. $( ... ) opens another shell, so cd
# does not affect the normal execution of the script.
SCRIPT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)

# Command to build with, eg. make or ninja.
BUILDER="make"
GENERATOR="" # Use "-G Ninja" for building with ninja.
KEEP_GOING_ON_ERRORS=false

IS_MAC=false
if [ "$(uname)" == "Darwin" ]; then
    IS_MAC=true
fi

# If the number of cores was not explicitly specified, use the number of cores minus one:
if [ "$IS_MAC" = true ] ; then
    NUM_PROC=`sysctl -n hw.ncpu`
else
    NUM_PROC=$(nproc)
fi

NUM_PROC=$(($NUM_PROC - 1))
# Make sure to have at least 1 build job.
NUM_PROC=$(($NUM_PROC > 1 ? $NUM_PROC : 1))

##########################################################################
# Run build commands
##########################################################################

cd "$SCRIPT_PATH"/..
mkdir -p build
cd build

cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS="ON"                                      \
    ..

if [ "$KEEP_GOING_ON_ERRORS" = true ]; then
    if [ "$BUILDER" = "ninja" ]; then
        "$BUILDER" -k 0 -j"$NUM_PROC"
    else
        "$BUILDER" -k -j"$NUM_PROC"
    fi
else
    "$BUILDER" -j"$NUM_PROC"
fi
