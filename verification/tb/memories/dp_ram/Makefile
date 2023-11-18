#common code
#Common code inclusion for verilator testbench
include ${VERIF_COMMON_ROOT}/clocking/common_clocking.makefile
include ${VERIF_COMMON_ROOT}/testbench/common_testbench.makefile

TOP_LEVEL := dp_ram

TB_OBJS += ${COMMON_CLOCKING_OBJS}
# TB_OBJS += ${COMMON_TB_OBJS}
INCS += ${COMMON_CLOCKING_INCS}
INCS += ${COMMON_TB_INCS}

VERILATOR_CONFIG += ${TARGET_PATH}/verilator_config.vlt
VERILOG_SRC += ${LOGICAL_ROOT}/memories/dp_ram.sv 
VERILATOR_FLAGS += --clk CLKA
VERILATOR_FLAGS += --clk CLKB

TB_SRCS += ${TARGET_PATH}/tb_cpp/tb.cpp


$(TARGET_WORK_DIR)/%.o: ${TARGET_PATH}/tb_cpp/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS)  $(INCS) ${COMMON_CLOCKING_INCS} $(COMMON_TB_INCS)-c $< -o $@

VERILATOR_TARGET_NAME=dp_ram
