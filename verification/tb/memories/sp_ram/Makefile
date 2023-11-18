VERILATOR_CONFIG += ${TARGET_PATH}/verilator_config.vlt
VERILOG_SRC += ${LOGICAL_ROOT}/memories/sp_ram.sv
VERILATOR_FLAGS += --clk CLK

TB_SRCS += ${TARGET_PATH}/tb_cpp/tb.cpp

$(TARGET_WORK_DIR)/%.o: ${TARGET_PATH}/tb_cpp/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(INCS) -c $< -o $@

VERILATOR_TARGET_NAME=sp_ram
