##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2013 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################

lib_LIBRARIES = libLPT.a

libLPT_a_CXXFLAGS = @MPICH_CFLAGS@ @LPT_CFLAGS@ \
                   -I$(top_builddir)/src/LPT \
                   -I$(top_builddir)/src/DS \
                   -I$(top_builddir)/src/PP

libLPT_a_SOURCES = \
   DS/Communicator.C \
   DS/DSlib.C \
   DS/DV3.C \
   DS/DataBlock.C \
   DS/DecompositionManager.C \
   LPT/CSV_Output.C \
   LPT/FileManager.C \
   LPT/LPT.C \
   LPT/LPT_ParticleOutput.C \
   LPT/FV_ParticlePathOutput.C \
   LPT/LPT_ParticleInput.C \
   LPT/FV_ParticlePathBinaryOutput.C \
   PP/Circle.C \
   PP/StartPoint.C \
   PP/SimpleStartPointFactory.C \
   PP/Point.C \
   PP/PPlib.C \
   PP/Line.C \
   PP/MovingPoints.C \
   PP/PP_Transport.C \
   PP/Cuboid.C \
   PP/Rectangle.C \
   PP/Utility.C \
   PP/Interpolator.C \
   PP/PP_Integrator.C \
   PP/ParticleData.C \
   DS/Cache.h \
   DS/Communicator.h \
   DS/DSlib.h \
   DS/DV3.h \
   DS/DataBlock.h \
   DS/DecompositionManager.h \
   DS/CommDataBlock.h \
   LPT/CSV_Output.h \
   LPT/FileManager.h \
   LPT/FileOutput.h \
   LPT/FV_ParticlePathBinaryOutput.h \
   LPT/FV_ParticlePathOutput.h \
   LPT/LPT_Args.h \
   LPT/LPT_LogOutput.h \
   LPT/LPT_ParticleInput.h \
   LPT/LPT_ParticleOutput.h \
   LPT/LPT.h \
   LPT/PMlibWrapper.h \
   PP/Circle.h \
   PP/StartPoint.h \
   PP/SimpleStartPointFactory.h \
   PP/Point.h \
   PP/PPlib.h \
   PP/Line.h \
   PP/MovingPoints.h \
   PP/PP_Transport.h \
   PP/Cuboid.h \
   PP/Rectangle.h \
   PP/Utility.h \
   PP/Interpolator.h \
   PP/PP_Integrator.h \
   PP/ParticleData.h


LPT_includedir = $(includedir)
LPT_include_HEADERS = \
    $(top_builddir)/src/LPT/LPT_Args.h \
    $(top_builddir)/src/LPT/LPT.h

