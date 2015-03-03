##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################

AUTOMAKE_OPTIONS = subdir-objects

lib_LIBRARIES = libLPT.a

libLPT_a_CXXFLAGS = @MPICH_CFLAGS@ @LPT_CFLAGS@ @PDM_CFLAGS@\
                   -I$(top_builddir)/src/LPT \
                   -I$(top_builddir)/src/DS \
                   -I$(top_builddir)/src/PP

libLPT_a_SOURCES = \
   DS/Communicator.C \
   DS/DSlib.C \
   DS/DataBlock.C \
   DS/DecompositionManager.C \
   LPT/LPT.C \
   PP/StartPointCircle.C \
   PP/StartPoint.C \
   PP/StartPointPoint.C \
   PP/PPlib.C \
   PP/StartPointLine.C \
   PP/StartPointMovingPoints.C \
   PP/PP_Transport.C \
   PP/StartPointCuboid.C \
   PP/StartPointRectangle.C \
   PP/Utility.C \
   PP/Interpolator.C \
   PP/PP_Integrator.C \
   PP/ParticleData.C \
   PP/ParticleContainerIterator.C \
   DS/Cache.h \
   DS/Communicator.h \
   DS/DSlib.h \
   DS/DataBlock.h \
   DS/DecompositionManager.h \
   DS/CommDataBlock.h \
   LPT/MPI_Manager.h \
   LPT/LPT_Args.h \
   LPT/LPT_LogOutput.h \
   LPT/LPT.h \
   LPT/PMlibWrapper.h \
   PP/StartPointCircle.h \
   PP/StartPoint.h \
   PP/StartPointPoint.h \
   PP/PPlib.h \
   PP/StartPointLine.h \
   PP/StartPointMovingPoints.h \
   PP/PP_Transport.h \
   PP/StartPointCuboid.h \
   PP/StartPointRectangle.h \
   PP/Utility.h \
   PP/Interpolator.h \
   PP/PP_Integrator.h \
   PP/ParticleContainer.h \
   PP/ParticleContainerIterator.h \
   PP/ParticleData.h \
   PP/StartPointAll.h


LPT_includedir = $(includedir)
LPT_include_HEADERS = \
    $(top_builddir)/src/LPT/LPT_Args.h \
    $(top_builddir)/src/LPT/LPT.h

