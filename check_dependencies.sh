#!/bin/bash

exists() {
    if ! command -v $1 &> /dev/null
    then
        echo $1 'could not be found -- please install it and rerun the script'
        exit 1
    else
        echo "Found $1!"
    fi

}

exists verilator 
exists python
exists bender
exists gcc
exists g++
exists make
exists cpp
exists cmake
exists java
exists clang

