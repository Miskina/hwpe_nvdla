#!/bin/bash

usage()
{
    echo "
        Usage: $0 [-d]
        
        -d      Check the required dependencies if using the Docker container as the build environment. 
        
        " 1>&2;
        
        exit 1;
}

USING_DOCKER=0

while getopts "d" flag; do
    case "$flag" in
        d) # Using docker as build environment
            USING_DOCKER=1
            ;;
        *) # Any other option
            usage
            ;;
    esac
done


exists() {
    if ! command -v $1 &> /dev/null
    then
        echo $1 'could not be found -- please install it and rerun the script'
        exit 1
    else
        echo "Found $1!"
    fi

}

if [ "$USING_DOCKER" -eq "0" ]; then
    exists verilator
    exists cmake
    exists java
    exists clang
    exists perl
else
    exists docker
    exists docker-compose
fi

exists cpp
exists make
exists gcc
exists g++
exists python
exists bender
exists git
