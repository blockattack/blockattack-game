#! /bin/bash
set -e
set -x
set -u
set -o pipefail

mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL/releases/download/release-2.30.2/SDL2-2.30.2.tar.gz | tar -zx && cd SDL2-2.30.2 && ls -lrt
cd /staging/deps/SDL2-2.30.2 && ./configure --enable-shared --enable-static && make && make install

#https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.2/SDL2_image-2.6.2.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-2.8.2.tar.gz | tar -zx && cd SDL2_image-2.8.2 && ls -lrt
cd /staging/deps/SDL2_image-2.8.2 && ./configure --enable-shared --enable-static && make && make install

#https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8./SDL2_mixer-2.8.0.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-2.8.0.tar.gz | tar -zx && cd SDL2_mixer-2.8.0 && ls -lrt
cd /staging/deps/SDL2_mixer-2.8.0 && ./configure --enable-shared --enable-static && make && make install

#https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-2.22.0.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-2.22.0.tar.gz | tar -zx && cd SDL2_ttf-2.22.0 && ls -lrt
cd /staging/deps/SDL2_ttf-2.22.0 && ./configure --enable-shared --enable-static && make && make install
mkdir -p /staging/blockattack-game

mkdir -p /staging/deps && cd /staging/deps && curl https://icculus.org/physfs/downloads/physfs-3.0.2.tar.bz2 | tar -jx && cd physfs-3.0.2 && ls -lrt
cd /staging/deps/physfs-3.0.2 && cmake . && make && make install

# boost
cd ~
mkdir -p Downloads
cd Downloads
curl https://files.poulsander.com/~poul19/public_files/boost_1_65_1.tar.bz2 -O
tar xvfj boost_1_65_1.tar.bz2
pushd boost_1_65_1
./bootstrap.sh --with-libraries=program_options
./b2 install -j 2 --prefix=/usr link=static
popd
curl https://files.poulsander.com/~poul19/public_files/fmt-8.1.0.tar.gz | tar -xz
pushd fmt-8.1.0
cmake . -DFMT_TEST=OFF -DFMT_INSTALL=ON -DFMT_MASTER_PROJECT=OFF && make && make install
popd
