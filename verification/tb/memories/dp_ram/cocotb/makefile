# Makefile

# defaults
SIM ?= verilator
TOPLEVEL_LANG ?= verilog

EXTRA_ARGS += --trace-fst --trace-structs

VERILOG_SOURCES += $(PWD)/tb.sv
VERILOG_SOURCES += $(LOGICAL_ROOT)/memories/dp_ram.sv
# use VHDL_SOURCES for VHDL files

# TOPLEVEL is the name of the toplevel module in your Verilog or VHDL file
TOPLEVEL = tb

# MODULE is the basename of the Python test file
MODULE = tb

# include cocotb's make rules to take care of the simulator setup
include $(shell cocotb-config --makefiles)/Makefile.sim
