cmake_minimum_required(VERSION 3.8)
project(periph_to_csb_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
  message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()

# SystemC dependencies
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

# Find SystemC using SystemC's CMake integration
# find_package(SystemCLanguage QUIET)
add_executable(periph_to_csb_sim sim_periph_to_csb.cpp)


# target_compile_definitions(periph_to_csb_sim PUBLIC VM_SC=1 VM_TRACE=1)
target_compile_definitions(periph_to_csb_sim PUBLIC VM_TRACE=1)

# target_include_directories(periph_to_csb_sim PRIVATE ../common/cc ../common)
target_include_directories(periph_to_csb_sim PRIVATE ../common/sc ../common)

# add_library(Vperiph_to_csb_lib OBJECT)
add_library(Vcsb_slave_lib     STATIC)
# add_library(verilator_models   STATIC $<TARGET_OBJECTS:Vperiph_to_csb_lib> $<TARGET_OBJECTS:Vcsb_slave_lib>)

# target_link_libraries(periph_to_csb_sim PRIVATE verilator_models)
# target_link_libraries(periph_to_csb_sim PRIVATE $<TARGET_OBJECTS:Vcsb_slave_lib>)
target_link_libraries(periph_to_csb_sim PRIVATE Vcsb_slave_lib)

verilate(Vcsb_slave_lib TRACE SYSTEMC
    INCLUDE_DIRS "../../src"
    VERILATOR_ARGS -Os -x-assign 0 -Wno-UNOPTFLAT -Wno-UNUSED
    SOURCES ../../src/csb_slave.sv)


add_dependencies(periph_to_csb_sim Vcsb_slave_lib)
# set(CSB_SLAVE_HOME $<TARGET_FILE:Vcsb_slave_lib>)
# target_include_directories(periph_to_csb_sim $<TARGET_FILE_DIR:Vcsb_slave_lib>)
# message(STATUS "CSB Slave verilated and saved to ${CSB_SLAVE_HOME}")

verilate(periph_to_csb_sim TRACE SYSTEMC
    INCLUDE_DIRS "../src ../test ../intf"
    VERILATOR_ARGS -Os -x-assign 0 -Wno-UNOPTFLAT -Wno-UNUSED --exe
    SOURCES ../../src/periph_to_csb_wrapper.sv
            ../../../src/periph_to_csb.sv
            ../../../intf/hwpe_ctrl_intf_periph.sv
            ../../../intf/nvdla_csb_intf.sv
            ../../../intf/nvdla_dbb_intf.sv)


# verilator_link_systemc(periph_to_csb_sim)
