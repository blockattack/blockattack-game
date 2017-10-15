#! /bin/bash
set -e
set -x

mkdir -p /staging/deps && cd /staging/deps && curl https://libsdl.org/release/SDL2-2.0.6.tar.gz | tar -zx && cd SDL2-2.0.6 && ls -lrt
cd /staging/deps/SDL2-2.0.6 && ./configure --enable-shared --enable-static && make && make install

#https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.1.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.1.tar.gz | tar -zx && cd SDL2_image-2.0.1 && ls -lrt
cd /staging/deps/SDL2_image-2.0.1 && ./configure --enable-shared --enable-static && make && make install

#https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.tar.gz | tar -zx && cd SDL2_mixer-2.0.1 && ls -lrt
cd /staging/deps/SDL2_mixer-2.0.1 && ./configure --enable-shared --enable-static && make && make install

#https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.tar.gz | tar -zx && cd SDL2_ttf-2.0.14 && ls -lrt
cd /staging/deps/SDL2_ttf-2.0.14 && ./configure --enable-shared --enable-static && make && make install
mkdir -p /staging/blockattack-game

mkdir -p /staging/deps && cd /staging/deps && curl http://icculus.org/physfs/downloads/physfs-2.0.3.tar.bz2 | tar -jx && cd physfs-2.0.3 && ls -lrt
cd /staging/deps/physfs-2.0.3 && cmake . && make && make install
