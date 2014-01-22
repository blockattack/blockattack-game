GAMEDIR=../../Game/

BINARY=$(GAMEDIR)blockattack

ifndef CC
CC=gcc
endif

#ifndef CPP
CPP=g++
#endif

BASE_CFLAGS=-c $(shell sdl-config --cflags)

ifndef BUILDDIR
BUILDDIR=build
endif

BASE_LIBS=$(shell sdl-config --libs) -lSDL_image -lSDL_mixer -lSDL_ttf

#For developement only 
ifndef DEBUG
DEBUG=0
endif

ifndef NETWORK
NETWORK=1
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

OFILES=main.o highscore.o ReadKeyboard.o joypad.o listFiles.o replay.o common.o stats.o CppSdlException.o CppSdlImageHolder.o Libs/NFont.o MenuSystem.o menudef.o

$(BINARY): 	$(OFILES)
	$(CPP) -O -o $(BINARY) $(OFILES) $(BASE_LIBS)
#-lphysfs

%.o : %.cpp
	g++ -MD $(BASE_CFLAGS) -c -o $@ $<
	@cp $*.d $*.P; \
             sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
                 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
             rm -f $*.d

-include *.P


run: $(BINARY)
	cd $(GAMEDIR) && ./blockattack

clean:
	rm *.o *.P
