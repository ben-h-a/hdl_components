add_library( common_testbench
    base_testbench.cpp
)


target_include_directories(common_testbench
    PUBLIC
    ${COMMON_CLOCKING}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VERILATOR_ROOT}/include
    ${VERILATOR_ROOT}/include/vltstd
)