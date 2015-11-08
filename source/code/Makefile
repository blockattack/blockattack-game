GAMEDIR=../../Game/

BINARY=$(GAMEDIR)blockattack

ifndef CC
CC=gcc
endif

ifdef CROSS
CXX=$(CROSS)g++
endif

BASE_CFLAGS=-c $(shell $(CROSS)sdl-config --cflags) -std=c++11

ifndef BUILDDIR
BUILDDIR=build
endif

BASE_LIBS=$(shell $(CROSS)sdl-config --libs) $(shell $(CROSS)pkg-config SDL_image SDL_mixer SDL_ttf --libs) 

#For developement only 
ifndef DEBUG
DEBUG=0
endif

ifndef NETWORK
NETWORK=0
endif

#Compile with debug information or optimized.
ifeq ($(DEBUG),1)
BASE_CFLAGS += -g -DDEBUG=1 -Wall
else
BASE_CFLAGS += -O3 
endif

ifeq ($(NETWORK),1)
BASE_CFLAGS += -DNETWORK=1
BASE_LIBS += -lenet
else
BASE_CFLAGS += -DNETWORK=0
endif

BASE_LIBS += -lphysfs

OFILES=main.o highscore.o ReadKeyboard.o joypad.o listFiles.o common.o stats.o CppSdlException.o CppSdlImageHolder.o Libs/NFont.o MenuSystem.o menudef.o puzzlehandler.o

ifeq ($(CROSS),i686-pc-mingw32-)
OFILES += winicon.res
BINARY := $(BINARY).exe
endif

$(BINARY): 	$(OFILES)
	$(CXX) -O -o $(BINARY) $(OFILES) $(BASE_LIBS)
#-lphysfs

winicon.res:	winicon.rc
	$(CROSS)windres winicon.rc -O coff -o winicon.res

%.o : %.cpp
	$(CXX) -MD $(BASE_CFLAGS) -c -o $@ $<
	@cp $*.d $*.P; \
             sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
                 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
             rm -f $*.d

-include *.P


run: $(BINARY)
	cd $(GAMEDIR) && ./blockattack

clean:
	rm *.o *.P Libs/*.o Libs/*.P
