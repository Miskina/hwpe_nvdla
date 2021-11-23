cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

set(NVDLA_PROJECT "nv_small")

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
  message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()

option(HWPE_NVDLA_USE_SYSTEMC      "Use SystemC simulations (CURRENTLY NOT SUPPORTED!)"    OFF)

if (DEFINED $ENV{HWPE_NVDLA_ROOT})
  set(HWPE_NVDLA_ROOT_DIR $ENV{HWPE_NVDLA_ROOT})
else()
  get_filename_component(HWPE_NVDLA_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
endif(DEFINED $ENV{HWPE_NVDLA_ROOT})

set(HWPE_NVDLA_SUBMODULE ${HWPE_NVDLA_ROOT_DIR}/nvdla_hw)
set(HWPE_NVDLA_SUBMODULE_VMOD ${HWPE_NVDLA_SUBMODULE}/vmod)
set(HWPE_NVDLA_SOURCE_DIR ${HWPE_NVDLA_ROOT_DIR}/src)
set(HWPE_NVDLA_INTF_DIR ${HWPE_NVDLA_ROOT_DIR}/intf)
set(HWPE_NVDLA_TEST_DIR ${HWPE_NVDLA_ROOT_DIR}/test)
set(HWPE_NVDLA_TEST_SOURCES_DIR ${HWPE_NVDLA_TEST_DIR}/src)
set(HWPE_NVDLA_VERILATOR_DIR ${HWPE_NVDLA_TEST_DIR}/verilator)
set(HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE ${HWPE_NVDLA_VERILATOR_DIR}/common)
set(HWPE_NVDLA_DEPENDENCIES_DIR ${HWPE_NVDLA_ROOT_DIR}/dependencies)
set(HWPE_NVDLA_AXI2MEM_DIR ${HWPE_NVDLA_DEPENDENCIES_DIR}/axi2mem)
set(HWPE_NVDLA_AXI_DIR ${HWPE_NVDLA_DEPENDENCIES_DIR}/axi)
set(HWPE_NVDLA_VERILATOR_FILE ${HWPE_NVDLA_VERILATOR_DIR}/verilator.f)
set(HWPE_NVDLA_SMALL_VERILATOR_FILE ${HWPE_NVDLA_VERILATOR_DIR}/verilator_nv_small.f)

if(HWPE_NVDLA_USE_SYSTEMC)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    find_package(SystemCLanguage QUIET)
    set(HWPE_NVDLA_VERILATOR_COMMON_DIR ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/sc) 
else()
    set(HWPE_NVDLA_VERILATOR_COMMON_DIR ${HWPE_NVDLA_VERILATOR_COMMON_DIR_BASE}/cc) 
endif(HWPE_NVDLA_USE_SYSTEMC)

file(GLOB HWPE_NVDLA_ALL_SOURCES    "${HWPE_NVDLA_SOURCE_DIR}/*.sv")
file(GLOB HWPE_NVDLA_ALL_INTERFACES "${HWPE_NVDLA_INTF_DIR}/*.sv")

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

# file(GLOB HWPE_NVDLA_AXI2MEM_SRC_FILES "${HWPE_NVDLA_AXI2MEM_DIR/*.sv")
# add_custom_command(COMMAND ./${PROJECT_NAME}
#                    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
#                    DEPENDS ${PROJECT_NAME}
# )

# function(add_run_script TARGET_NAME)
    #    get_target_property(_EXECUTABLE_NAME ${TARGET_NAME} LOCATION)
    #    set(_EXECUTABLE_NAME $<TARGET_FILE:${TARGET_NAME}>)
    #    get_property(_EXECUTABLE_NAME TARGET ${TARGET_NAME} PROPERTY LOCATION)
    #    configure_file(${HWPE_NVDLA_VERILATOR_DIR}/templates/run.sh.in ${CMAKE_CURRENT_BINARY_DIR}/run.sh @ONLY)
    #    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/run.sh CONTENT "!#/bin/bash\n\n$<TARGET_FILE:${TARGET_NAME}> \"@\"") 
    #    add_custom_command(TARGET ${TARGET_NAME}
    #                       POST_BUILD
    #                       DEPENDS ${CMAKE_BINARY_DIR}/run.sh
    #                       COMMAND echo "GENERATING run for $<TARGET_FILE:${TARGET_NAME}>" & chmod +x ${CMAKE_BINARY_DIR}/run.sh
    #                       VERBATIM
    #    )
#     add_custom_target(run_script chmod +x ${CMAKE_CURRENT_BINARY_DIR}/run.sh
#                       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/run.sh)
#     add_dependencies(run_script ${PROJECT_NAME})
    
    #     file(WRITE ${CMAKE_PROJECT_DIR}/run_simulation.cmake
    #         "cmake_minimum_required(VERSION 3.8)\n\noption(ARGS \"Arguments passed to the program\" \"\")\n\nexecute_process(COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${ARGS} WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}"
    #         TARGET ${PROJECT_NAME}
    #     )
#    add_custom_target(run
#        COMMAND ${PROJECT_NAME} $(RUN_ARGS)
#        DEPENDS ${PROJECT_NAME}
#        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
#    )
# endfunction(add_run_script) 

# add_run_target()
