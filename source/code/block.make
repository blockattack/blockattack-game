GAMEDIR=../../Game/

BINARY=$(GAMEDIR)blockattack

CC=gcc

CPP=g++

BASE_CFLAGS=-c $(shell sdl-config --cflags)


BASE_LIBS=$(shell sdl-config --libs) -lSDL_image -lSDL_mixer 
#-lSDL_ttf

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

$(BINARY): 	build/main.o build/highscore.o build/SFont.o build/ReadKeyboard.o build/joypad.o build/listFiles.o build/replay.o build/common.o build/stats.o
	$(CPP) -O -o $(BINARY) build/main.o build/highscore.o build/SFont.o build/ReadKeyboard.o build/joypad.o build/listFiles.o build/replay.o build/common.o build/stats.o $(BASE_LIBS)
#-lphysfs

build/main.o:	main.cpp mainVars.hpp common.h
	$(CPP) $(BASE_CFLAGS) main.cpp -o build/main.o

build/blockgame.o:	BlockGame.hpp BlockGame.cpp
	$(CPP) $(BASE_CFLAGS) BlockGame.cpp -o build/blockgame.o

build/highscore.o: highscore.h highscore.cpp
	$(CPP) $(BASE_CFLAGS) highscore.cpp -o build/highscore.o

build/SFont.o: SFont.h SFont.c
	$(CC) $(BASE_CFLAGS) SFont.c -o build/SFont.o

build/ReadKeyboard.o: ReadKeyboard.h ReadKeyboard.cpp
	$(CPP) $(BASE_CFLAGS) ReadKeyboard.cpp -o build/ReadKeyboard.o

build/joypad.o: joypad.h joypad.cpp
	$(CPP) $(BASE_CFLAGS) joypad.cpp -o build/joypad.o

build/listFiles.o: listFiles.h listFiles.cpp
	$(CPP) $(BASE_CFLAGS) listFiles.cpp -o build/listFiles.o

build/replay.o: replay.h replay.cpp
	$(CPP) $(BASE_CFLAGS) replay.cpp -o build/replay.o

build/stats.o: stats.h stats.cc
	$(CPP) $(BASE_CFLAGS) stats.cc -o build/stats.o

build/common.o: common.h common.cc
	$(CPP) $(BASE_CFLAGS) common.cc -o build/common.o

#build/uploadReplay.o: uploadReplay.cc uploadReplay.h
#	$(CPP) $(BASE_CFLAGS) uploadReplay.cc -o build/uploadReplay.o

#build/MenuSystem.o: MenuSystem.cc MenuSystem.h
#	$(CPP) $(BASE_CFLAGS) MenuSystem.cc -o build/MenuSystem.o

#build/ttfont.o: ttfont.h ttfont.cc
#	$(CPP) $(BASE_CFLAGS) ttfont.cc -o build/ttfont.o


run: $(BINARY)

clean:
	rm build/*o
