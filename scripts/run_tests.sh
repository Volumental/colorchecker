#!/bin/bash
set -eu

# Safe way to find location of the script, using $0 does not
# work when script is sourced. First the path to the script
# is retrieved from BASH_SOURCE. After changing to this dir
# (resolves relative paths, etc.) pwd gives the absolute path
# to the script dir. $( ... ) opens another shell, so cd
# does not affect the normal execution of the script.
SCRIPT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)

cd build
ctest
