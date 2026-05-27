#! /bin/bash
set -e
set -x
set -u
set -o pipefail

# SDL3 (uses CMake, not autotools)
mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-3.4.8.tar.gz | tar -zx && cd SDL3-3.4.8 && ls -lrt
cd /staging/deps/SDL3-3.4.8 && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DSDL_SHARED=ON -DSDL_STATIC=ON && cmake --build build && cmake --install build

mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.4/SDL3_image-3.4.4.tar.gz | tar -zx && cd SDL3_image-3.4.4 && ls -lrt
cd /staging/deps/SDL3_image-3.4.4 && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DSDLIMAGE_SHARED=ON -DSDLIMAGE_STATIC=ON && cmake --build build && cmake --install build

mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.2/SDL3_mixer-3.2.2.tar.gz | tar -zx && cd SDL3_mixer-3.2.2 && ls -lrt
cd /staging/deps/SDL3_mixer-3.2.2 && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DSDLMIXER_SHARED=ON -DSDLMIXER_STATIC=ON && cmake --build build && cmake --install build

mkdir -p /staging/deps && cd /staging/deps && curl -L https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz | tar -zx && cd SDL3_ttf-3.2.2 && ls -lrt
cd /staging/deps/SDL3_ttf-3.2.2 && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DSDLTTF_SHARED=ON -DSDLTTF_STATIC=ON && cmake --build build && cmake --install build

mkdir -p /staging/blockattack-game

mkdir -p /staging/deps && cd /staging/deps && curl https://icculus.org/physfs/downloads/physfs-3.0.2.tar.bz2 | tar -jx && cd physfs-3.0.2 && ls -lrt
cd /staging/deps/physfs-3.0.2 && cmake . && make && make install

# boost
cd ~
mkdir -p Downloads
cd Downloads
curl https://files.poulsander.com/~poul19/public_files/boost_1_89_0.tar.bz2 -O
tar xvfj boost_1_89_0.tar.bz2
pushd boost_1_89_0
./bootstrap.sh --with-libraries=program_options
./b2 install -j 2 --prefix=/usr link=static
popd
curl https://files.poulsander.com/~poul19/public_files/fmt-8.1.0.tar.gz | tar -xz
pushd fmt-8.1.0
cmake . -DFMT_TEST=OFF -DFMT_INSTALL=ON -DFMT_MASTER_PROJECT=OFF && make && make install
popd
