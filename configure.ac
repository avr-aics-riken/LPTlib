##############################################################################
#
# LPTlib - Lagrangian Particle Tracking library
# 
# Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
# All right reserved.
#
##############################################################################
#
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([LPTlib], [1.0.0], [keno@riken.jp], [LPTlib])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([src/LPT/LPT.C])
AC_CONFIG_HEADERS([config.h])


# revision No.
LPT_REVISION="20141009_0800"
AC_SUBST(LPT_REVISION)


#
# platform
#
LPT_DEVICE="$($SHELL ./lpt-uname)"
AC_SUBST(LPT_DEVICE)


#
# compiler libs
#
AC_ARG_WITH(comp, [AC_HELP_STRING([--with-comp=(INTEL|FJ)],[Specify Compiler type])], , with_comp=none)
AC_SUBST(COMPILER_LIBS)
case "$with_comp" in
"INTEL")
  COMPILER_LIBS="-lifport -lifcore"
  ;;
"FJ")
  COMPILER_LIBS="--linkfortran"
  ;;
esac

enable_mpi="none"


#
# MPICH environment
#
AC_SUBST(MPICH_DIR)
AC_SUBST(MPICH_CFLAGS)
AC_SUBST(MPICH_LDFLAGS)
AC_SUBST(MPICH_LIBS)
AC_SUBST(MPICH_FCLIBS)
AC_ARG_WITH(mpich, [AC_HELP_STRING([--with-mpich=dir],[Specify MPICH install directory])], , with_mpich=none)
if test "$with_mpich" != "none" ; then
  MPICH_DIR=$with_mpich;
  MPICH_CFLAGS=-I$MPICH_DIR/include
  MPICH_LDFLAGS=-L$MPICH_DIR/lib
  enable_mpi="mpich"
fi

#
# OpenMPI environment
#
if test "$with_mpich" = "none" ; then
  AC_ARG_WITH(ompi, [AC_HELP_STRING([--with-ompi=dir],[Specify OpenMPI install directory])], , with_ompi=none)
  if test "$with_ompi" != "none" ; then
    MPICH_DIR=$with_ompi;
    MPICH_CFLAGS=-I$MPICH_DIR/include
    MPICH_LDFLAGS=-L$MPICH_DIR/lib
    enable_mpi="ompi"
  fi
fi


#
# MPI Library
#
if test "$with_comp" = "FJ" ; then
  MPICH_LIBS=""
  case "$host" in
  sparc*)
    MPICH_FCLIBS="$MPICH_LIBS"" -lmpi_f77 -lmpi_f90"
    ;;
  esac
elif test "$enable_mpi" = "mpich" ; then
  MPICH_LIBS="-lmpich"
  MPICH_FCLIBS="$MPICH_LIBS"" -lfmpich"
  case "$CPM_DEVICE" in
  "Lion"|"Snow_Leopard"|"Leopard"|"Mountain_Lion")
#    MPICH_LIBS="$MPICH_LIBS"" -lpmpich"
    ;;
  esac
elif test "$enable_mpi" = "ompi" ; then
  MPICH_LIBS="-lmpi"
  MPICH_FCLIBS="$MPICH_LIBS"" -lmpi_f77 -lmpi_f90"
  case "$CPM_DEVICE" in
  "Lion"|"Snow_Leopard"|"Leopard"|"Mountain_Lion")
#    MPICH_LIBS="$MPICH_LIBS"" -lmpi_cxx"
    ;;
  esac
fi



#
# default prefix setting
#
AC_PREFIX_DEFAULT([/usr/local/LPTlib])
if test "$prefix" = "NONE" ; then
  LPT_INST_DIR=/usr/local/LPTlib
else
  LPT_INST_DIR="$prefix"
fi


#
# REAL_TYPE
#
AC_ARG_WITH(realtype, [AC_HELP_STRING([--with-realtype=(float|double)],[Specify REAL_TYPE])], , with_realtype=float)
AC_SUBST(LPT_REALTYPE)
if test "$with_realtype" = "float" ; then
  LPT_REALTYPE="-DREAL_TYPE=float"
else
  LPT_REALTYPE="-DREAL_TYPE=double"
fi


#
# CAHCHE_CONTAINER
#
AC_ARG_WITH(container, [AC_HELP_STRING([--with-container=(deque|list)],[Specify Cache Container type])], , with_container=deque)
AC_SUBST(LPT_CONTAINER)
if test "$with_container" = "list" ; then
  LPT_CONTAINER="-DCACHE_CONTAINER=list"
else
  LPT_CONTAINER="-DCACHE_CONTAINER=deque"
fi


#
# LOG
#
AC_ARG_WITH(log, [AC_HELP_STRING([--with-log=(disable|enable)],[Specify log mode])], , with_log=disable)
AC_SUBST(LPT_LOG)
if test "$with_log" = "enable" ; then
  LPT_LOG=-DLPT_LOG_ENABLE
else
  LPT_LOG=
fi


#
# PMlib environment
#
AC_SUBST(PM_DIR)
AC_SUBST(PM_CFLAGS)
AC_ARG_WITH(pm, [AC_HELP_STRING([--with-pm=dir],[Specify PMlib install directory])], , with_pm=none)
AC_SUBST(PM_LDFLAGS)
AC_SUBST(PM_LIBS)
if test "$with_pm" != "none" ; then
  PM_DIR=$with_pm;
  PM_CFLAGS="-I$PM_DIR/include -DUSE_PMLIB"
  PM_LDFLAGS=-L$PM_DIR/lib
  PM_LIBS=$PM_DIR/lib/libPM.a
else
  echo "  Error: PM library not specified."
  echo "         set --with-pm option"
  exit 1
fi


#
# LPT special flags
#
LPT_CFLAGS="-I$LPT_INST_DIR/include $LPT_REALTYPE $LPT_CONTAINER $LPT_LOG $PM_CFLAGS"
AC_SUBST(LPT_CFLAGS)

LPT_LDFLAGS="-L$LPT_INST_DIR/lib"
AC_SUBST(LPT_LDFLAGS)

LPT_LIBS="-lLPT"
AC_SUBST(LPT_LIBS)

LPT_CXX="$CXX"
AC_SUBST(LPT_CXX)

LPT_FC="$FC"
AC_SUBST(LPT_FC)

LPT_FC_LD="$FC"
if test "$with_comp" = "FJ"; then
  LPT_FC_LD="$CXX"" --linkfortran"
else
  LPT_LIBS="$LPT_LIBS"" -lstdc++"
fi
AC_SUBST(LPT_FC_LD)


#
# MAKE_SUB_DIRS
#
AC_SUBST(MAKE_SUB_DIRS)
MAKE_SUB_DIRS="src FileConverter doc"



# Checks for programs.
AC_PROG_CXX
AC_PROG_FC
AC_PROG_RANLIB


# Checks for libraries.
AC_CHECK_LIB(PM, start)


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h \
                  stdlib.h \
                  sys/time.h \
                 ])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_C_CONST
AC_HEADER_TIME


# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt gettimeofday])


AC_CONFIG_FILES([Makefile \
                 src/Makefile \
                 FileConverter/Makefile \
                 lpt-config \
                 doc/Makefile \
                ])

AC_OUTPUT

chmod +x ./lpt-config

