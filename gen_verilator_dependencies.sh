#!/usr/bin/env bash

for d in dependencies/*/; do
    verilator_file_name="$(basename -- ${d%/}).f"
    bender script verilator > "$d/$verilator_file_name"
done