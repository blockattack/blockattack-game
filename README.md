# blockattack-game [![Build Status](https://travis-ci.org/blockattack/blockattack-game.svg?branch=master)](https://travis-ci.org/blockattack/blockattack-game) [![Coverity Status](https://scan.coverity.com/projects/8278/badge.svg)](https://scan.coverity.com/projects/8278)
Block Attack - Rise of the Blocks - the game<br/>
A Tetris Attack Clone under the GPL.

Homepage: http://www.blockattack.net<br/>
Source: https://github.com/blockattack/blockattack-game

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
  * libboost-program-options
  * libutfcpp (only needed for compiling)
  * jsoncpp

# Building
The only supported build method is using the Makefile<br/>
To build do:
```
./packdata.sh
cmake .
# or
# cmake -DCMAKE_BUILD_TYPE=Debug .
# or
# cmake -DCMAKE_BUILD_TYPE=Release .
make
```
The result should be in the "Game"-folder. To run
```
./Game/blockattack
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
i686-w64-mingw32.static-cmake -DCMAKE_BUILD_TYPE=Release . && make
```

# Reporting bugs

Please report bugs on github: https://github.com/blockattack/blockattack-game/issues
If possible: Check if the bug is already fixed in master. But if you don't know how to check it then just report it.

# Contributions
Contributions are welcome. Both the current graphics and the original sharedir support was provided by kind individuals. 
Patches can be attached to a github ticket or sent to poul@poulsander.com directly. You can also create pull requests on Github. 
