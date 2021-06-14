#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

bender script verilator > $SCRIPT_DIR/test/verilator/verilator.f
