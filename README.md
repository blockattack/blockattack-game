# blockattack-game [![Build Status](https://travis-ci.org/blockattack/blockattack-game.svg?branch=master)](https://travis-ci.org/blockattack/blockattack-game) [![Coverity Status](https://scan.coverity.com/projects/8278/badge.svg)](https://scan.coverity.com/projects/8278) [![license](https://img.shields.io/github/license/blockattack/blockattack-game.svg)]()
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
The only supported build method is using the CMake<br/>
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

# Building using Docker

As getting a C++ project with many dependencies to compile can be a daunting task then I have provided a couple of Docker images that can perform a build. Both for Windows and Linux.

On a fresh checkout you can use:
```
docker build -f source/misc/docker/Dockerfile.Ubuntu14.04build . -t blockattack_test
```
and
```
docker build -f source/misc/docker/Dockerfile.WindoesBuild . -t blockattack_test
```


# Source Structure
This project is a bit unconversionel because I didn't know any better at the time.

 * Game - The output is placed here
 * man - The manual file and the script to generate it. May be moved to source/misc at some point
 * source/code/ - The source code
 * source/code/Libs - External libs that are compiled into the project because they are either header only (Cereal) or not designed for use as a shared library (NFont).
 * source/code/sago - Source code. Not designed to be specific to Block Attack - Rise of the Blocks
 * source/assets - Source for the assets if relevant. For instance svg source for the graphics.
 * source/misc - Misc stuff. Code related tool that are used for development but not part of the final product.
 * source/misc/astyle - Helper script that enforces code style using the "astyle"-program
 * source/misc/cmake - Files needed by CMake
 * source/misc/icons - Icons for the installer
 * source/misc/screenshots - Screen shots like the one in this README file.
 * source/misc/translation - Translations and related tools and scripts
 * source/misc/travis_help - Helper files for Travis CI
 * windows installer - Information needed to Windows installer. May be moved to source/misc at some point

# Reporting bugs

Please report bugs on github: https://github.com/blockattack/blockattack-game/issues
If possible: Check if the bug is already fixed in master. But if you don't know how to check it then just report it.

# Contributions
Contributions are welcome. Both the current graphics and the original sharedir support was provided by kind individuals.
Patches can be attached to a github ticket or sent to poul@poulsander.com directly. You can also create pull requests on Github.
