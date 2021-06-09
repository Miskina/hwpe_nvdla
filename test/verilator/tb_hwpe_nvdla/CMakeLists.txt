cmake_minimum_required(VERSION 3.8)
project(tb_hwpe_nvdla)

include(${CMAKE_CURRENT_LIST_DIR}/../hwpe_nvdla.cmake)

add_executable(${PROJECT_NAME} sim_hwpe_nvdla.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/FabricController.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR}/PeriphController.cpp
                   ${HWPE_NVDLA_VERILATOR_COMMON_DIR}/TcdmMemoryController.cpp)
 
target_include_directories(${PROJECT_NAME} PRIVATE ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE} ${HWPE_NVDLA_VERILATOR_COMMON_DIR})

target_compile_definitions(${PROJECT_NAME} PUBLIC VM_TRACE=1)

verilate(${PROJECT_NAME} TRACE
    INCLUDE_DIRS ${HWPE_NVDLA_SOURCE_DIR} ${HWPE_NVDLA_TEST_SOURCES_DIR} ${HWPE_NVDLA_INTF_DIR} ${HWPE_NVDLA_AXI2MEM_DIR}
    VERILATOR_ARGS -Os -x-assign 0 -Wno-UNOPTFLAT -Wno-UNUSED --trace-depth 3 -f verilator_nv_small.f -f ${HWPE_NVDLA_AXI2MEM_VERILATOR_FILE} -f ${HWPE_NVDLA_AXI_DIR}/verilator.f
                  -CFLAGS "-fPIE -DPIE" --compiler gcc --output-split 250000000 --top tb_hwpe_nvdla
    SOURCES ${HWPE_NVDLA_TEST_SOURCES_DIR}/${PROJECT_NAME}.sv
            ${HWPE_NVDLA_ALL_SOURCES}
            ${HWPE_NVDLA_ALL_INTERFACES})

add_dependencies(${PROJECT_NAME} nvdla_trace_tests)
