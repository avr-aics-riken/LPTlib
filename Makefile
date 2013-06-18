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

.SUFFIXES:.c .f .F .f90 .F90
.c.o:
	$(CC) $(CCFLAGS) -c -o$@ $<
.C.o:
	$(CXX) $(CXXFLAGS) -c -o$@ $<
.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o$@ $<
.f.o:
	$(FC) $(FFLAGS) -c -o$@ $<
.F.o:
	$(FC) $(FFLAGS) -c -o$@ $<
.f90.o:
	$(FC) $(FFLAGS) -c -o$@ $<
.F90.o:
	$(FC) $(FFLAGS) -c -o$@ $<


.PHONY: clean doc test lib FileConverter all
