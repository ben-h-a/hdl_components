set(TARGET sp_ram)
set(VERIATOR_CONF ${CMAKE_CURRENT_SOURCE_DIR}/verilator_config.vlt)
set(VERIATOR_ARGS --clk CLK)

project(${TARGET})

# Add testbenches in the tb directory
file(GLOB_RECURSE TB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tb_cpp/*.cpp")

# Add executable target for component1_name
add_executable(${TARGET} ${TB_SOURCES})
           
# Add the Verilated circuit to the target
verilate(${TARGET}
    DIRECTORY ${BUILD_DIR}
    INCLUDE_DIRS 
    SOURCES ${VERILATOR_CONF} 
            ${LOGICAL_ROOT}/memories/sp_ram.sv
    TRACE
    VERILATOR_ARGS ${VERIATOR_ARGS} ${VERIATOR_DEFAULT_ARGS}
  )