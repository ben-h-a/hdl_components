include ${VERIF_COMMON_ROOT}/clocking/common_clocking.makefile

COMMON_TB_DIR := $(VERIF_COMMON_ROOT)/testbench
COMMON_TB_OBJS_DIR := ${COMMON_TB_DIR}/objs

COMMON_TB_INCS += -I${COMMON_TB_DIR}/
COMMON_TB_SRCS :=
