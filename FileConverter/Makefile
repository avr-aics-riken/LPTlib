##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2013 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################

include ../make_setting

CXXFLAGS += -I../src/LPT -I../src/DS -I../src/PP

LDFLAGS += -L$(LPT_DIR)/lib -lLPT
LDFLAGS += -L$(PMLIB_DIR)/lib -lPM

SRCS_CPP = FileConverterID.C FileConverterTime.C main.C
OBJS=$(SRCS_CPP:%.C=%.o)

LM = FileConverter

all:$(LM)

$(LM):$(OBJS)
	$(LINKER) $(OBJS) $(LDFLAGS) -o$@ 
	-mkdir -p $(LPT_DIR)/bin
	mv $(LM) $(LPT_DIR)/bin

clean:
	-rm -rf $(OBJS) $(LM)
