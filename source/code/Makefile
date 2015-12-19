GAMEDIR=../../Game/

BINARY=$(GAMEDIR)blockattack

-include sago/Makefile.sago

ifndef CC
CC=gcc
endif

ifdef CROSS
CXX=$(CROSS)g++
endif

BASE_CFLAGS=-c $(shell $(CROSS)sdl2-config --cflags) -std=c++11

ifndef BUILDDIR
BUILDDIR=build
endif

BASE_LIBS=$(shell $(CROSS)sdl2-config --libs) -lSDL_ttf $(shell $(CROSS)pkg-config SDL_image SDL_mixer --libs) $(SAGO_BASE_LIBS)

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

OFILES=main.o highscore.o ReadKeyboard.o joypad.o listFiles.o common.o stats.o CppSdlImageHolder.o Libs/NFont.o MenuSystem.o menudef.o puzzlehandler.o ${SAGO_O_FILES}

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

SOURCE_FILES = $(OFILES:.o=.cpp)

format:
	astyle -t -j -y -c -k1 -z2 -A2 --pad-header $(SOURCE_FILES) 
