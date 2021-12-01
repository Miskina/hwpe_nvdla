cmake_minimum_required(VERSION 3.8)
project(hwpe_nvdla_sw)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/arch)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

option(FIRMWARE "Build the NVDLA firmware" ON)

# add_subdirectory(firmware)
