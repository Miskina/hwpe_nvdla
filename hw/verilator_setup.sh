#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TARGET_ROOT=${1:-${SCRIPT_DIR}}

echo "[Verilator] Using bender to generate verilator file list with ${TARGET_ROOT} as the target"

SCRIPT_DIR_SED=$(echo ${SCRIPT_DIR} | sed 's/\//\\\//g')
TARGET_ROOT_SED=$(echo ${TARGET_ROOT} | sed 's/\//\\\//g')
bender script verilator -t rtl | sed -e "s/${SCRIPT_DIR_SED}/${TARGET_ROOT_SED}/g" > $SCRIPT_DIR/test/verilator/verilator.f
