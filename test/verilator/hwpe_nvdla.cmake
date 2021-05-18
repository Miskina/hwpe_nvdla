cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

set(NVDLA_PROJECT "nv_small")

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
  message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()


option(HWPE_NVDLA_USE_SYSTEMC      "Use SystemC simulations (CURRENTLY NOT SUPPORTED!)"    OFF)

get_filename_component(HWPE_NVDLA_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(HWPE_NVDLA_SUBMODULE ${HWPE_NVDLA_ROOT_DIR}/nvdla_hw)
set(HWPE_NVDLA_SUBMODULE_VMOD ${HWPE_NVDLA_SUBMODULE}/vmod)
set(HWPE_NVDLA_SOURCE_DIR ${HWPE_NVDLA_ROOT_DIR}/src)
set(HWPE_NVDLA_INTF_DIR ${HWPE_NVDLA_ROOT_DIR}/intf)
set(HWPE_NVDLA_TEST_DIR ${HWPE_NVDLA_ROOT_DIR}/test)
set(HWPE_NVDLA_TEST_SOURCES_DIR ${HWPE_NVDLA_TEST_DIR}/src)
set(HWPE_NVDLA_VERILATOR_DIR ${HWPE_NVDLA_TEST_DIR}/verilator)
set(HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE ${HWPE_NVDLA_VERILATOR_DIR}/common)

if(HWPE_NVDLA_USE_SYSTEMC)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    find_package(SystemCLanguage QUIET)
    set(HWPE_NVDLA_VERILATOR_COMMON_DIR ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/sc) 
else()
    set(HWPE_NVDLA_VERILATOR_COMMON_DIR ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/cc) 
endif(HWPE_NVDLA_USE_SYSTEMC)

file(GLOB HWPE_NVDLA_ALL_SOURCES    "${HWPE_NVDLA_SRC_DIR}/*.sv")
file(GLOB HWPE_NVDLA_ALL_INTERFACES "${HWPE_NVDLA_INTF_DIR}/*.sv")

# add_custom_target(generate)
message(STATUS "NVDLA project folder at: ${HWPE_NVDLA_SUBMODULE}")

file(GLOB NVDLA_TRACE_TEST_CFG_FOLDERS "${HWPE_NVDLA_SUBMODULE}/verif/tests/trace_tests/${NVDLA_PROJECT}/*")

foreach(NVDLA_TRACE_TEST_FOLDER ${NVDLA_TRACE_TEST_CFG_FOLDERS})
  get_filename_component(TRACE_TEST_NAME ${NVDLA_TRACE_TEST_FOLDER} NAME)  
  message(STATUS "Generating trace test ${TRACE_TEST_NAME}")
  set(NVDLA_TRACE "${HWPE_NVDLA_SUBMODULE}/outdir/${NVDLA_PROJECT}/verilator/test/${TRACE_TEST_NAME}/trace.bin")
  # Unfortunate but necessary if we want to use NVDLA-s Makefile to generate tests
  set(_NVDLA_IN_FOLDER_DEPTH "../..") 
  set(_NVDLA_TRACE_PATH_FOR_MAKEFILE "${_NVDLA_IN_FOLDER_DEPTH}/outdir/${NVDLA_PROJECT}/verilator/test/${TRACE_TEST_NAME}/trace.bin")
  add_custom_command(OUTPUT ${NVDLA_TRACE}
                     COMMAND make ${_NVDLA_TRACE_PATH_FOR_MAKEFILE} > /dev/null 
                     WORKING_DIRECTORY ${HWPE_NVDLA_SUBMODULE}/verif/verilator
                     DEPENDS ${NVDLA_TRACE_TEST_FOLDER})
  list(APPEND NVDLA_TRACE_TESTS ${NVDLA_TRACE})
endforeach(NVDLA_TRACE_TEST_FOLDER)

add_custom_target(nvdla_trace_tests
                  DEPENDS ${NVDLA_TRACE_TESTS})
