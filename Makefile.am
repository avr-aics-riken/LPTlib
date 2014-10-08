##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################

SUBDIRS = @MAKE_SUB_DIRS@

bin_SCRIPTS    = lpt-config
CLEANFILES     = $(bin_SCRIPTS)
EXTRA_DIST     = lpt-config.in LICENSE Makefile_hand make_setting src/Makefile_hand \
                 src/Doxyfile src/depend.inc lpt-uname
dist_data_DATA = AUTHORS COPYING ChangeLog README LICENSE

clean-local:
	find ./ -name *.obj | xargs rm -f
	find ./ -name *.exe | xargs rm -f
	find ./ -name *.lib | xargs rm -f

distclean-local: clean-local
	rm -f ./bin/*
	rm -f ./lib/*

