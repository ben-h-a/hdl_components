cmake_minimum_required(VERSION 3.20)

project(common_components_verif)
cmake_policy(SET CMP0074 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)  # or whichever version you need
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LOGICAL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../logical)
set(COMMON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(COMMON_CLOCKING ${COMMON_ROOT}/clocking)
set(COMMON_TESTBENCH ${COMMON_ROOT}/testbench)
set(VERIATOR_DEFAULT_ARGS -Wall --Wno-fatal --sv)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})


# set(VERILATOR_DEFAULT_ARGS "--main --timing")

add_subdirectory(common/clocking)
add_subdirectory(common/testbench)
add_subdirectory(tb/memories/sp_ram)
add_subdirectory(tb/memories/dp_ram)