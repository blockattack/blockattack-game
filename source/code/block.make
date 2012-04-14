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
DEBUG=1
endif

ifndef NETWORK
NETWORK=1
endif

#Compile with debug information or optimized.
ifeq ($(DEBUG),1)
BASE_CFLAGS += -g -DDEBUG=1
else
BASE_CFLAGS += -O4
endif

ifeq ($(NETWORK),1)
BASE_CFLAGS += -DNETWORK=1
BASE_LIBS += -lenet
else
BASE_CFLAGS += -DNETWORK=0
endif

BASE_LIBS += -lphysfs

$(BINARY): 	$(BUILDDIR)/main.o $(BUILDDIR)/highscore.o $(BUILDDIR)/ReadKeyboard.o $(BUILDDIR)/joypad.o $(BUILDDIR)/listFiles.o $(BUILDDIR)/replay.o $(BUILDDIR)/common.o $(BUILDDIR)/stats.o $(BUILDDIR)/CppSdlException.o $(BUILDDIR)/CppSdlImageHolder.o $(BUILDDIR)/nfont.o $(BUILDDIR)/MenuSystem.o $(BUILDDIR)/menudef.o
	$(CPP) -O -o $(BINARY) $(BUILDDIR)/main.o $(BUILDDIR)/highscore.o  $(BUILDDIR)/ReadKeyboard.o $(BUILDDIR)/joypad.o $(BUILDDIR)/listFiles.o $(BUILDDIR)/replay.o $(BUILDDIR)/common.o $(BUILDDIR)/stats.o $(BUILDDIR)/CppSdlException.o $(BUILDDIR)/CppSdlImageHolder.o  $(BUILDDIR)/nfont.o $(BUILDDIR)/MenuSystem.o $(BUILDDIR)/menudef.o $(BASE_LIBS)
#-lphysfs

$(BUILDDIR)/main.o:	main.cpp mainVars.hpp common.h
	$(CPP) $(BASE_CFLAGS) main.cpp -o $(BUILDDIR)/main.o

$(BUILDDIR)/blockgame.o:	BlockGame.hpp BlockGame.cpp
	$(CPP) $(BASE_CFLAGS) BlockGame.cpp -o $(BUILDDIR)/blockgame.o

$(BUILDDIR)/highscore.o: highscore.h highscore.cpp
	$(CPP) $(BASE_CFLAGS) highscore.cpp -o $(BUILDDIR)/highscore.o


$(BUILDDIR)/ReadKeyboard.o: ReadKeyboard.h ReadKeyboard.cpp
	$(CPP) $(BASE_CFLAGS) ReadKeyboard.cpp -o $(BUILDDIR)/ReadKeyboard.o

$(BUILDDIR)/joypad.o: joypad.h joypad.cpp
	$(CPP) $(BASE_CFLAGS) joypad.cpp -o $(BUILDDIR)/joypad.o

$(BUILDDIR)/listFiles.o: listFiles.h listFiles.cpp
	$(CPP) $(BASE_CFLAGS) listFiles.cpp -o $(BUILDDIR)/listFiles.o

$(BUILDDIR)/replay.o: replay.h replay.cpp
	$(CPP) $(BASE_CFLAGS) replay.cpp -o $(BUILDDIR)/replay.o

$(BUILDDIR)/stats.o: stats.h stats.cc
	$(CPP) $(BASE_CFLAGS) stats.cc -o $(BUILDDIR)/stats.o

$(BUILDDIR)/common.o: common.h common.cc
	$(CPP) $(BASE_CFLAGS) common.cc -o $(BUILDDIR)/common.o

$(BUILDDIR)/nfont.o: Libs/NFont.h Libs/NFont.cpp
	$(CPP) $(BASE_CFLAGS) Libs/NFont.cpp -o $(BUILDDIR)/nfont.o

$(BUILDDIR)/menudef.o: menudef.cpp
	$(CPP) $(BASE_CFLAGS) menudef.cpp -o $(BUILDDIR)/menudef.o

#$(BUILDDIR)/uploadReplay.o: uploadReplay.cc uploadReplay.h
#	$(CPP) $(BASE_CFLAGS) uploadReplay.cc -o $(BUILDDIR)/uploadReplay.o

$(BUILDDIR)/MenuSystem.o: MenuSystem.cc MenuSystem.h
	$(CPP) $(BASE_CFLAGS) MenuSystem.cc -o $(BUILDDIR)/MenuSystem.o

#$(BUILDDIR)/ttfont.o: ttfont.h ttfont.cc
#	$(CPP) $(BASE_CFLAGS) ttfont.cc -o $(BUILDDIR)/ttfont.o

$(BUILDDIR)/CppSdlImageHolder.o: CppSdlImageHolder.hpp CppSdlImageHolder.cpp
	$(CPP) $(BASE_CFLAGS) -o $(BUILDDIR)/CppSdlImageHolder.o CppSdlImageHolder.cpp

$(BUILDDIR)/CppSdlException.o: CppSdlException.hpp CppSdlException.cpp
	$(CPP) $(BASE_CFLAGS) -o $(BUILDDIR)/CppSdlException.o CppSdlException.cpp


run: $(BINARY)

clean:
	rm $(BUILDDIR)/*o
