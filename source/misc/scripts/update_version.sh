#! /bin/bash 
set -e
set +H
if [ "$#" -lt 3 ]; then
	echo "Must be called with $0 <MAJOR> <MINOR> <PATCH> [>TAG>]"
	echo "Like: $0 2 2 10 SNAPSHOT"
	exit 1
fi
MAJOR=$1
MINOR=$2
PATCH=$3
TAG=$4

FULLVERSION=$MAJOR.$MINOR.$PATCH

if [ "$#" -gt 3 ]; then
FULLVERSION="$FULLVERSION-$TAG"
fi

sed -i "/SET(CPACK_PACKAGE_VERSION /c\\SET(CPACK_PACKAGE_VERSION \"$FULLVERSION\")" CMakeLists.txt
sed -i "/SET(CPACK_PACKAGE_VERSION_MAJOR /c\\SET(CPACK_PACKAGE_VERSION_MAJOR \"$MAJOR\")" CMakeLists.txt
sed -i "/SET(CPACK_PACKAGE_VERSION_MINOR /c\\SET(CPACK_PACKAGE_VERSION_MINOR \"$MINOR\")" CMakeLists.txt
sed -i "/SET(CPACK_PACKAGE_VERSION_PATCH /c\\SET(CPACK_PACKAGE_VERSION_PATCH \"$PATCH\")" CMakeLists.txt


sed -i -E "s/#define VERSION_NUMBER.*$/#define VERSION_NUMBER \"$FULLVERSION\"/" source/code/version.h
sed -i -E "s/!define PRODUCT_VERSION .*$/!define PRODUCT_VERSION \"$FULLVERSION\"/" "windows installer/install_script.nsi"
sed -i -E "s/#ENV BLOCKATTACK_VERSION .*$/#ENV BLOCKATTACK_VERSION $FULLVERSION/" source/misc/docker/Dockerfile.WindoesBuild

make
pushd man
COLUMNS=300 help2man --no-info --section=6 --name="a puzzle game inspired by Tetris Attack" ../Game/blockattack | sed 's@'"$HOME"'@$HOME@g' > blockattack.man
popd
