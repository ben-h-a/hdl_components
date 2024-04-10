cmake_minimum_required(VERSION 3.20)

project(common_components_verif)
cmake_policy(SET CMP0074 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)  # or whichever version you need
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Set optimization levels for different build types
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Debug build - disable optimization
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Release build - enable optimization
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()


set(LOGICAL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../logical)
set(COMMON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(COMMON_CLOCKING ${COMMON_ROOT}/clocking)
set(COMMON_TESTBENCH ${COMMON_ROOT}/testbench)
set(VERIATOR_DEFAULT_ARGS -Wall --trace --Wno-fatal --sv --debug )

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})


# set(VERILATOR_DEFAULT_ARGS "--main --timing")

add_subdirectory(common/clocking)
add_subdirectory(common/testbench)
add_subdirectory(tb/memories/sp_ram)
add_subdirectory(tb/memories/dp_ram)