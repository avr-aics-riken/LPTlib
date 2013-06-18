include ../Mkinclude.LPT
CXXFLAGS += -I$(LPT_DIR)/src/LPT/ -I$(LPT_DIR)/src/DSlib/ -I$(LPT_DIR)/src/PPlib/

#source files for LPT library
LIB_SRCS_CPP=\
LPT/LPT.C \
LPT/FileManager.C \
LPT/LPT_ParticleOutput.C \
LPT/LPT_ParticleInput.C  \
LPT/FV_ParticlePathOutput.C \
LPT/FV_ParticlePathBinaryOutput.C \
LPT/CSV_Output.C \
DSlib/Communicator.C \
DSlib/DecompositionManager.C \
DSlib/DSlib.C \
DSlib/DataBlock.C \
DSlib/DV3.C \
PPlib/ParticleData.C \
PPlib/Utility.C \
PPlib/StartPoint.C \
PPlib/MovingPoints.C \
PPlib/Line.C \
PPlib/Rectangle.C \
PPlib/Cuboid.C \
PPlib/Circle.C \
PPlib/PPlib.C \
PPlib/PP_Integrator.C \
PPlib/PP_Transport.C \
PPlib/Interpolator.C \


LIBDIR=$(LPT_DIR)/lib
INCDIR=$(LPT_DIR)/include

LIB_OBJS=$(LIB_SRCS_CPP:%.C=%.o)
all: $(LIBNAME)

depend:
	makedepend -f depend.inc -Y./ -- $(CXXFLAGS) -- $(LIB_SRCS_CPP) 2>/dev/null

doc:
	doxygen Doxyfile

$(LIBNAME):$(LIB_OBJS)
	-mkdir -p $(LIBDIR)
	$(AR) $(LIBNAME) $(LIB_OBJS)
	mv $(LIBNAME) $(LIBDIR)
	-mkdir -p $(INCDIR)
	cp LPT/LPT.h LPT/LPT_Args.h $(INCDIR)


clean:
	-rm -rf $(LIB_OBJS) $(LIBDIR)/$(LIBNAME)



include depend.inc
