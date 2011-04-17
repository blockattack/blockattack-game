#CppSdl/Makefile

ifndef CPP
CPP=g++
endif

OUTDIR=../build

ODIR=$(OUTDIR)/

BASE_CFLAGS=-c $(shell sdl-config --cflags)
BASE_LIBS=$(shell sdl-config --libs) -lSDL_image -lSDL_mixer 

DEBUG=1

#Compile with debug information or optimized.
ifeq ($(DEBUG),1)
BASE_CFLAGS += -g -DDEBUG=1
else
BASE_CFLAGS += -O4
endif

total: $(ODIR)/CppSdlImageHolder.o $(ODIR)/CppSdlException.o

$(ODIR)/CppSdlImageHolder.o: CppSdlImageHolder.hpp CppSdlImageHolder.cpp
	$(CPP) $(BASE_CFLAGS) -o $(ODIR)/CppSdlImageHolder.o CppSdlImageHolder.cpp


$(ODIR)/CppSdlException.o: CppSdlException.hpp CppSdlException.cpp
	$(CPP) $(BASE_CFLAGS) -o $(ODIR)/CppSdlException.o CppSdlException.cpp
