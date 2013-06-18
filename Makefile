##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2013 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################


all: lib FileConverter

lib:
	$(MAKE) -C src

FileConverter: lib
	$(MAKE) -C FileConverter

doc:
	$(MAKE) doc -C src

clean: cleanlib cleanFileConverter

cleanlib:
	$(MAKE) clean -C src

cleanFileConverter:
	$(MAKE) clean -C FileConverter
  
depend:
	$(MAKE) depend -C src

.SUFFIXES:.c .f .F .f90 .F90

.c.o:
	$(CC) $(CCFLAGS) -c -o$@ $<
  
.C.o:
	$(CXX) $(CXXFLAGS) -c -o$@ $<


.PHONY: clean doc test lib depend FileConverter all
