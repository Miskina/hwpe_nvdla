cmake_minimum_required(VERSION 3.8)
project(verilator_tbs)

file(GLOB ALL_TBS "tb_*/")
include(hwpe_nvdla.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/arch)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)

foreach(TESTBENCH ${ALL_TBS})
    add_subdirectory(${TESTBENCH})
endforeach(TESTBENCH ${ALL_TBS})

add_subdirectory(cpp_tests)
