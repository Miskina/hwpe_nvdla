cmake_minimum_required(VERSION 3.8)

# Name of the project
project(nvdla_periph)

if (NOT DEFINED ENV{HWPE_NVDLA_ROOT})
    message(FATAL_ERROR "Please define the HWPE_NVDLA_ROOT environment variable")
endif ()

if (NOT DEFINED ENV{RENODE_VERILATOR_INTEGRATION_ROOT})
    message(FATAL_ERROR "Please define the RENODE_VERILATOR_INTEGRATION_ROOT environment variable")
endif ()

set(NVDLA_VERILATOR_FLAGS "-f $ENV{HWPE_NVDLA_ROOT}/hw/test/verilator/verilator_nv_small.f")
set(NVDLA_TOP $ENV{HWPE_NVDLA_ROOT}/hw/nvdla/outdir/nv_small/vmod/nvdla/top/NV_nvdla.v)

# Verilog file containing the top module to be Verilated
set(VTOP ${NVDLA_TOP})

# C/C++ source files to be compiled
set(CSOURCES nvdla.cpp)

# Additional compiling, linking or verilating arguments (respectively)
set(VERI_ARGS ${NVDLA_VERILATOR_FLAGS})
set(VERI_EXEC_ARGS ${NVDLA_VERILATOR_FLAGS})
set(VERI_LIB_ARGS ${NVDLA_VERILATOR_FLAGS})

# CMake file doing the hard job
include($ENV{RENODE_VERILATOR_INTEGRATION_ROOT}/cmake/configure-and-verilate.cmake)
