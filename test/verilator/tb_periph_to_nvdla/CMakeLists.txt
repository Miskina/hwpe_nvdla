cmake_minimum_required(VERSION 3.8)
project(tb_periph_to_nvdla)

# include(${CMAKE_CURRENT_LIST_DIR}/../hwpe_nvdla.cmake)

add_executable(tb_periph_to_nvdla sim_periph_to_nvdla.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/FabricController.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR}/PeriphController.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR}/AxiMemoryController.cpp)
 
target_include_directories(tb_periph_to_nvdla PRIVATE ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE} ${HWPE_NVDLA_VERILATOR_COMMON_DIR})

target_compile_definitions(tb_periph_to_nvdla PUBLIC VM_TRACE=1)

 
verilate(tb_periph_to_nvdla TRACE
    INCLUDE_DIRS ${HWPE_NVDLA_SOURCE_DIR} ${HWPE_NVDLA_TEST_SOURCES_DIR} ${HWPE_NVDLA_INTF_DIR}
    VERILATOR_ARGS -Os -x-assign 0 -Wno-UNOPTFLAT -Wno-UNUSED --trace-depth 3 -f ${HWPE_NVDLA_SMALL_VERILATOR_FILE}
                  -CFLAGS "-fPIE -DPIE" --compiler gcc --output-split 250000000 
    SOURCES ${HWPE_NVDLA_TEST_SOURCES_DIR}/tb_periph_to_nvdla.sv
            ${HWPE_NVDLA_SOURCE_DIR}/nvdla.sv
            ${HWPE_NVDLA_SOURCE_DIR}/periph_to_csb.sv
            ${HWPE_NVDLA_ALL_INTERFACES})

add_dependencies(tb_periph_to_nvdla nvdla_trace_tests)
