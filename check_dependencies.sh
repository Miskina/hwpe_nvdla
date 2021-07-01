#!/bin/bash

exists() {
    if ! command -v $1 &> /dev/null
    then
        echo $1 'could not be found -- please install it and rerun the script'
        exit
    fi

}

exists verilator 
exists python3
exists bender
exists cmake

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"


