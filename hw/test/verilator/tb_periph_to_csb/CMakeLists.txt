cmake_minimum_required(VERSION 3.8)
project(tb_periph_to_csb)

# include(${CMAKE_CURRENT_LIST_DIR}/../hwpe_nvdla.cmake)

add_executable(${PROJECT_NAME} sim_tb_periph_to_csb.cpp
	           ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/Util.h
		   ${HWPE_NVDLA_VERILATOR_COMMON_DIR}/PeriphMaster.h)
 
target_include_directories(${PROJECT_NAME} PRIVATE ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE} ${HWPE_NVDLA_VERILATOR_COMMON_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE VM_TRACE=1)
 
verilate(${PROJECT_NAME} TRACE
    INCLUDE_DIRS ${HWPE_NVDLA_SOURCE_DIR} ${HWPE_NVDLA_TEST_SOURCES_DIR} ${HWPE_NVDLA_INTF_DIR}
    VERILATOR_ARGS -Os -x-assign 0 -Wno-UNOPTFLAT -Wno-UNUSED --compiler gcc --output-split 250000000 -CFLAGS "-fPIE -DPIE"
    SOURCES ${HWPE_NVDLA_TEST_SOURCES_DIR}/tb_periph_to_csb.sv
            ${HWPE_NVDLA_TEST_SOURCES_DIR}/csb_slave_with_intf.sv
            ${HWPE_NVDLA_SOURCE_DIR}/periph_to_csb.sv
            ${HWPE_NVDLA_ALL_INTERFACES})
