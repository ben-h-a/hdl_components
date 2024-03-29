add_library( common_testbench INTERFACE)


target_include_directories(common_testbench
    INTERFACE
    ${COMMON_CLOCKING}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VERILATOR_ROOT}/include
    ${VERILATOR_ROOT}/include/vltstd
)