../../Game/blockattack: 	main.o highscore.o SFont.o ReadKeyboard.o joypad.o listFiles.o replay.o common.o stats.o uploadReplay.o
	g++ -O -o ../../Game/blockattack main.o highscore.o SFont.o ReadKeyboard.o joypad.o listFiles.o replay.o common.o stats.o `sdl-config --cflags --libs` -lSDL_image -lSDL_mixer -lenet
#-lphysfs

main.o:	main.cpp BlockGame.hpp mainVars.hpp common.h
	g++ -g -c main.cpp `sdl-config --cflags`

highscore.o: highscore.h highscore.cpp
	g++ -g -c highscore.cpp

SFont.o: SFont.h SFont.c
	gcc -g -c SFont.c `sdl-config --cflags`

ReadKeyboard.o: ReadKeyboard.h ReadKeyboard.cpp
	g++ -g -c ReadKeyboard.cpp `sdl-config --cflags`

joypad.o: joypad.h joypad.cpp
	g++ -g -c joypad.cpp `sdl-config --cflags`

listFiles.o: listFiles.h listFiles.cpp
	g++ -g -c listFiles.cpp

replay.o: replay.h replay.cpp
	g++ -g -c replay.cpp `sdl-config --cflags`

stats.o: stats.h stats.cc
	g++ -g -c stats.cc

common.o: common.h common.cc
	g++ -g -c common.cc

uploadReplay.o: uploadReplay.cc uploadReplay.h
	g++ -g -c uploadReplay.cc `sdl-config --cflags`

run: ../../Game/blockattack

clean:
	rm *o
