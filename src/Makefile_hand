##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2013 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################

include ../make_setting

CXXFLAGS += -I./LPT -I./DS -I./PP

#source files for LPT library
LIB_SRCS_CPP = \
               LPT/LPT.C \
               LPT/FileManager.C \
               LPT/LPT_ParticleOutput.C \
               LPT/LPT_ParticleInput.C  \
               LPT/FV_ParticlePathOutput.C \
               LPT/FV_ParticlePathBinaryOutput.C \
               LPT/CSV_Output.C \
               DS/Communicator.C \
               DS/DecompositionManager.C \
               DS/DSlib.C \
               DS/DataBlock.C \
               DS/DV3.C \
               PP/ParticleData.C \
               PP/Utility.C \
               PP/StartPoint.C \
               PP/MovingPoints.C \
               PP/Line.C \
               PP/Rectangle.C \
               PP/Cuboid.C \
               PP/Circle.C \
               PP/PPlib.C \
               PP/PP_Integrator.C \
               PP/PP_Transport.C \
               PP/Interpolator.C \


LIBDIR = $(LPT_DIR)/lib
INCDIR = $(LPT_DIR)/include

LIB_OBJS = $(LIB_SRCS_CPP:%.C=%.o)

all: $(LIBNAME)

depend:
	makedepend -f depend.inc -Y./ -- $(CXXFLAGS) -- $(LIB_SRCS_CPP) 2>/dev/null

doc:
	doxygen Doxyfile

$(LIBNAME):$(LIB_OBJS)
	$(AR) $(LIBNAME) $(LIB_OBJS)
	$(RANLIB) $(LIBNAME)
	-mkdir -p $(LIBDIR)
	mv $(LIBNAME) $(LIBDIR)
	-mkdir -p $(INCDIR)
	cp LPT/LPT.h LPT/LPT_Args.h $(INCDIR)


clean:
	-rm -rf $(LIB_OBJS) $(LIBNAME)

include depend.inc
