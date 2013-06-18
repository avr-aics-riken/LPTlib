include ../Mkinclude.LPT

CXXFLAGS += -I$(LPT_DIR)/src/LPT/ -I$(LPT_DIR)/src/DSlib/ -I$(LPT_DIR)/src/PPlib/

LDFLAGS+=-L$(LPT_DIR)/lib -lLPT
#PMlibを使用する場合は、以下の一行のコメントアウトを外して有効にする
LDFLAGS+= -L$(PMLIB_DIR)/lib -lpmlib

SRCS_CPP=FileConverterID.C FileConverterTime.C main.C
OBJS=$(SRCS_CPP:%.C=%.o)

LM=FileConverter

all:$(LM)

$(LM):$(OBJS)
	$(LINKER) $(OBJS) $(LDFLAGS) -o$@ 
	-mkdir -p ../bin
	mv $(LM) ../bin/

clean:
	-rm -rf $(OBJS) ../bin/$(LM)
