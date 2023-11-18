include ${VERIF_COMMON_ROOT}/clocking/common_clocking.makefile

COMMON_TB_DIR := $(VERIF_COMMON_ROOT)/testbench
COMMON_TB_OBJS_DIR := ${COMMON_TB_DIR}/objs

COMMON_TB_INCS += -I${COMMON_TB_DIR}/
# COMMON_TB_SRCS := ${COMMON_TB_DIR}/base_testbench.cpp
COMMON_TB_SRCS :=

# COMMON_TB_OBJS += $(patsubst %.cpp, $(COMMON_TB_OBJS_DIR)/%.o, $(notdir $(COMMON_TB_SRCS)))

# $(COMMON_TB_OBJS_DIR)/%.o: ${COMMON_TB_DIR}/%.cpp
# 	@mkdir -p $(@D)
# 	$(CXX) $(CFLAGS) ${INCS} ${COMMON_CLOCKING_INCS} $(COMMON_TB_INCS) -c $< -o $@
