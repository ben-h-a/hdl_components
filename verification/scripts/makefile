#CLI args
TARGET:=memories/sp_ram
TOP_LEVEL=sp_ram
TARGET_PATH=${VERIFICATION_ROOT}/tb/${TARGET}
VERBOSE:=0
DEBUG:=0
DEFAULT_EXEC_NAME:=tb_exec
VCD:=

include conandeps.mk

ifeq ($(VERILATOR_ROOT),)
VERILATOR = verilator
VERILATOR_COVERAGE = verilator_coverage
VERILATOR_ROOT ?= $(shell bash -c 'verilator -V|grep VERILATOR_ROOT | head -1 | sed -e " s/^.*=\s*//"')
else
export VERILATOR_ROOT
VERILATOR = $(VERILATOR_ROOT)/bin/verilator
VERILATOR_COVERAGE = $(VERILATOR_ROOT)/bin/verilator_coverage
endif
ZLIB_PATH := /opt/homebrew/Cellar/zlib/1.3
ZLIB_LIBS := -I${ZLIB_PATH}include -L${ZLIB_PATH}/lib -lz
VERILATOR_INCS := -I /opt/homebrew/Cellar/zlib/1.3/include/ -I$(VERILATOR_ROOT)/include -I$(VERILATOR_ROOT)/include/vltstd

#compiler and flags
CXX := g++

LDFLAGS := -Wl,-U,__Z15vl_time_stamp64v,-U,__Z13sc_time_stampv
CFLAGS := -std=c++20 -Wall -Wextra -fdiagnostics-color=auto -faligned-new -g -D__WORDSIZE=64 -march=native
CFLAGS += $(CONAN_CXXFLAGS)

ifeq ($(VERBOSE),1)
CFLAGS+= -v
endif
ifeq ($(DEBUG),1)
CFLAGS+= -O0 -fno-inline -g
else
CFLAGS+= -Og
endif

ifneq ($(VCD),)
CFLAGS+= -DVCD_FILE=\"$(VCD)\"
endif
#
VERILOG_SRC := 
TB_SRCS :=
TB_OBJS :=
INCS := $(CONAN_LIBS)
VERILATOR_ARGS := -cc --Wall --build --trace

TARGET_WORK_DIR := ${TARGET_PATH}/work
VERILATOR_OBJS := ./verilator_objs

include ${TARGET_PATH}/makefile

VERILATOR_ARGS += --waiver-output $(TARGET_WORK_DIR)/verilator_waivers.vlt
INCS +=  ${VERILATOR_INCS} ${ZLIB_LIBS} -I$(TARGET_WORK_DIR)

TB_OBJS += $(patsubst %.cpp, $(TARGET_WORK_DIR)/%.o, $(notdir $(TB_SRCS)))

all: $(TOP_LEVEL)

objs:
	@ls ${TARGET_PATH}/tb_cpp/
	@echo $(VERILATOR_OBJ_FILES)
	@echo $(TARGET_WORK_DIR)/V${TOP_LEVEL}__ALL.o
	@echo $(TB_OBJS)

#Compile full object file for verilated objects
$(TARGET_WORK_DIR)/V${TOP_LEVEL}__ALL.o: $(VERILOG_SRC)
	@mkdir -p $(TARGET_WORK_DIR)
	${VERILATOR} --top ${TOP_LEVEL} ${VERILATOR_ARGS} --Mdir $(TARGET_WORK_DIR) --prefix V$(notdir $(basename $<)) ${VERILATOR_CONFIG} $<

$(VERILATOR_OBJS)/%.o: $(VERILATOR_ROOT)/include/%.cpp
	@mkdir -p $(VERILATOR_OBJS)
	$(CXX) $(CFLAGS) ${VERILATOR_INCS} -c $< -o $@

# $(VERILATOR_OBJS)/%.o: $(VERILATOR_ROOT)/include/gtkwave/%.c
# 	@mkdir -p $(VERILATOR_OBJS)
# 	$(CXX) $(CFLAGS) ${VERILATOR_INCS} -c $< -o $@

# VERILATOR_CPP_SOURCES := $(wildcard $(VERILATOR_ROOT)/include/gtkwave/*.c)
# VERILATOR_OBJ_FILES := $(patsubst %.c, $(VERILATOR_OBJS)/%.o, $(notdir $(VERILATOR_CPP_SOURCES)))
VERILATOR_CPP_SOURCES += $(wildcard $(VERILATOR_ROOT)/include/*.cpp)
VERILATOR_OBJ_FILES := $(patsubst %.cpp, $(VERILATOR_OBJS)/%.o, $(notdir $(VERILATOR_CPP_SOURCES)))

$(info VERILATOR_OBJ_FILES: $(VERILATOR_OBJ_FILES))
$(info TB_OBJS: $(TB_OBJS))
$(info TARGET_WORK_DIR: $(TARGET_WORK_DIR))
$(TOP_LEVEL): $(VERILATOR_OBJ_FILES) $(TARGET_WORK_DIR)/V${TOP_LEVEL}__ALL.o $(TB_OBJS) $(CONAN_LIBS)
	$(CXX) $(CFLAGS) $(INCS) $(LDFLAGS) $^ -o ${TARGET_WORK_DIR}/${DEFAULT_EXEC_NAME}

run: $(TOP_LEVEL)
	${TARGET_WORK_DIR}/$(DEFAULT_EXEC_NAME)

clean_target:
	rm -rf $(TARGET_WORK_DIR)
clean_verilator:
	rm -rf $(VERILATOR_OBJS)

clean_common:
	rm -rf ${VERIF_COMMON_ROOT}/**/objs

clean: clean_target clean_verilator clean_common

.PHONY: all run clean
