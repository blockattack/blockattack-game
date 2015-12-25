# blockattack-game [![Build Status](https://travis-ci.org/blockattack/blockattack-game.svg)](https://travis-ci.org/blockattack/blockattack-game)
Block Attack - Rise of the Blocks - the game
A Tetris Attack Clone under the GPL. 

# Dependencies
  * A version of g++ with C++11 support. Tested on g++-4.9
  * libSDL
  * libSDL2_image
  * libSDL2_mixer
  * libSDL2_ttf
  * libenet
  * libphysfs
  * libboost (only needed for compiling)
  * libutfcpp (only needed for compiling)
  * jsoncpp

# Building
The only supported build method is using the Makefile
To build do:
```
cd source/code
make
```
The result should be in the "Game"-folder. To run
```
cd ../../Game
./blockattack
```



