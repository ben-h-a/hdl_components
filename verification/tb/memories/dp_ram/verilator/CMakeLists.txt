set(TARGET dp_ram)
set(VERIATOR_ARGS 
    --clk CLKA 
    --clk CLKB 
    --top dp_ram
                )

project(${TARGET})

# Add testbenches in the tb directory
file(GLOB_RECURSE TB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tb_cpp/*.cpp")

add_executable(${TARGET} ${TB_SOURCES})

target_link_libraries(${TARGET} PRIVATE common_clocking common_testbench)
# target_include_directories(${TARGET}
#     PRIVATE
#     ${COMMON_CLOCKING}
#     ${COMMON_TESTBENCH}
# )

verilate(${TARGET}
    DIRECTORY ${BUILD_DIR}
    INCLUDE_DIRS
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/verilator_config.vlt
        ${LOGICAL_ROOT}/memories/dp_ram.sv
    TRACE
    PREFIX V${TARGET}
    VERILATOR_ARGS ${VERIATOR_ARGS} ${VERIATOR_DEFAULT_ARGS}
  )