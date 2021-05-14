cmake_minimum_required(VERSION 3.8)
project(cpp_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

add_executable(test_memory mem_test.cpp)
