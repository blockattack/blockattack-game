# blockattack-game [![Build Status](https://travis-ci.org/blockattack/blockattack-game.svg?branch=master)](https://travis-ci.org/blockattack/blockattack-game)
Block Attack - Rise of the Blocks - the game
A Tetris Attack Clone under the GPL. 

www.blockattack.net

##Screen shot
![Block Attack - Rise of the Blocks 2.0.0 snapshot](/source/misc/screenshots/screen_shot_2016_01_19.png?raw=true "Screen shot from 2016-01-19")

# Dependencies
  * A version of g++ with C++11 support. Tested on g++-4.9
  * libSDL2
  * libSDL2_image
  * libSDL2_mixer
  * libSDL2_ttf
  * libphysfs
  * libboost (only needed for compiling)
  * libutfcpp (only needed for compiling)
  * jsoncpp

# Building
The only supported build method is using the Makefile
To build do:
```
./packdata.sh
cmake .
make
```
The result should be in the "Game"-folder. To run
```
cd ./Game/blockattack
```

You can also choose to install it with
```
sudo make install
```

Windows build uses MXE (mxe.cc) with these installed:
```
sdl2 sdl2_image sdl2_mixer sdl2_ttf physfs jsoncpp libtool gettext freetype zlib boost
```
and libutfcpp copied from "source/misc/travis_help/utf8_v2_3_4/source/" to "/path/to/mxe/usr/lib/gcc/i686-w64-mingw32.static/4.9.3/include"

Compiled with:
```
i686-w64-mingw32.static-cmake --debug-output . && make
```
