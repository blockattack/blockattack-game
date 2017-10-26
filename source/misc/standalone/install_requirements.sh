#! /bin/bash
set -e
apt-get update && apt-get install -y build-essential cmake pkg-config libfreetype6-dev libvorbis-dev libgl1-mesa-dev libpulse-dev libpng12-dev zip gettext

apt-get update && apt-get install -y python-software-properties
add-apt-repository ppa:ubuntu-toolchain-r/test
apt-get update
apt-get install -y gcc-4.8 g++-4.8
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
