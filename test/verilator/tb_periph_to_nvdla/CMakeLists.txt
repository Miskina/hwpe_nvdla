cmake_minimum_required(VERSION 3.8)
project(sim_tb_periph_to_nvdla)

include(${CMAKE_CURRENT_LIST_DIR}/../common_vars.cmake)

add_executable(sim sim_periph_to_nvdla.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/AXIResponder.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/TraceLoader.cpp)
 
target_include_directories(sim PRIVATE ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE} ${HWPE_NVDLA_VERILATOR_COMMON_DIR})
 
verilate(sim TRACE
    INCLUDE_DIRS ${HWPE_NVDLA_SOURCE_DIR} ${HWPE_NVDLA_TEST_SOURCES_DIR} ${HWPE_NVDLA_INTF_DIR}
    VERILATOR_ARGS -Os -x-assign 0 -Wno-UNOPTFLAT -Wno-UNUSED --trace-depth 3 -f verilator_nv_small.f --timescale-override 1ps
    SOURCES ${HWPE_NVDLA_TEST_SOURCES_DIR}/tb_periph_to_nvdla.sv
            ${HWPE_NVDLA_ALL_SOURCES}
            ${HWPE_NVDLA_ALL_INTERFACES})