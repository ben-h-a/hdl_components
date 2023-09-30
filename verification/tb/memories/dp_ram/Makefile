#common code
#Common code inclusion for verilator testbench
include ${VERIF_COMMON_ROOT}/clocking/common_clocking.makefile
include ${VERIF_COMMON_ROOT}/testbench/common_testbench.makefile

VERILATOR_CONFIG += ${TARGET_PATH}/verilator_config.vlt
VERILATOR_INPUT += ${LOGICAL_ROOT}/memories/dp_ram.sv
VERILATOR_FLAGS += --clk CLK_A
VERILATOR_FLAGS += --clk CLK_B

COMPILER_INPUT += ${TARGET_PATH}/tb.cpp

VERILATOR_TARGET_NAME=dp_ram
