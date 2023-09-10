VERILATOR_FLAGS =
TARGET:=memories/sp_ram
TARGET_PATH=${VERIFICATION_ROOT}/tb/${TARGET}
include ${TARGET_PATH}/makefile

WORK_DIR = ${TARGET_PATH}/work
LOG_DIR = ${TARGET_PATH}/log
OUTPUT_VERILATOR_DIR = ${TARGET_PATH}/obj_dir

VERIF_SCRIPTS_ROOT = $(PWD)

ifeq ($(VERILATOR_ROOT),)
VERILATOR = verilator
VERILATOR_COVERAGE = verilator_coverage
else
export VERILATOR_ROOT
VERILATOR = $(VERILATOR_ROOT)/bin/verilator
VERILATOR_COVERAGE = $(VERILATOR_ROOT)/bin/verilator_coverage
endif

# Generate C++ in executable form
VERILATOR_FLAGS += --cc --exe
# Generate makefile dependencies (not shown as complicates the Makefile)
#VERILATOR_FLAGS += -MMD
# Optimize
VERILATOR_FLAGS += --Mdir ${OUTPUT_VERILATOR_DIR}
VERILATOR_FLAGS += --x-assign 0
# Warn abount lint issues; may not want this on less solid designs
VERILATOR_FLAGS += -Wall
# Make waveforms
#VERILATOR_FLAGS += --trace
# Check SystemVerilog assertions
VERILATOR_FLAGS += --assert
# Generate coverage analysis
VERILATOR_FLAGS += --coverage
# Run make to compile model, with as many CPUs as are free
VERILATOR_FLAGS += --build -j
# Run Verilator in debug mode
#VERILATOR_FLAGS += --debug
# Add this trace to get a backtrace in gdb
#VERILATOR_FLAGS += --gdbbt


work:
	mkdir -p ${WORK_DIR}

verilate: work
	@echo
	@echo "-- VERILATE ----------------"
	$(VERILATOR) --version
	$(VERILATOR) ${VERILATOR_FLAGS} ${VERILATOR_INPUT}



run: verilate
	@echo
	@echo "-- RUN ---------------------"
	@rm -rf ${LOG_DIR}
	@mkdir -p ${LOG_DIR}
	${OUTPUT_VERILATOR_DIR}/${VERILATOR_TARGET_NAME}

	@echo
	@echo "-- DONE --------------------"


######################################################################
# Other targets

show-config:
	$(VERILATOR) -V

genhtml:
	@echo "-- GENHTML --------------------"
	@echo "-- Note not installed by default, so not in default rule"
	$(GENHTML) logs/coverage.info --output-directory logs/html

maintainer-copy::
clean mostlyclean distclean maintainer-clean::
	cd ${TARGET_PATH};	rm -rf obj_dir logs *.log *.dmp *.vpd core;cd ${VERIF_SCRIPTS_ROOT}
