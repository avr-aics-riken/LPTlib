##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2013 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################

bin_PROGRAMS = FileConverter

FileConverter_SOURCES = \
   main.C

FileConverter_CXXFLAGS = @MPICH_CFLAGS@ @LPT_CFLAGS@ \
        -I$(top_builddir)/include \
        -I$(top_builddir)/src/LPT \
        -I$(top_builddir)/src/DS  \
        -I$(top_builddir)/src/PP

FileConverter_LDFLAGS = -L$(top_builddir)/src @PM_LDFLAGS@ @MPICH_LDFLAGS@
FileConverter_LDADD = $(top_builddir)/src/libLPT.a @PM_LIBS@ @MPICH_LIBS@



