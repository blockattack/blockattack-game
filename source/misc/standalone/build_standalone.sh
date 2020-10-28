#! /bin/bash
set -e

if [ "$#" -ne 1 ]; then
	echo "Must be called with $0 <archivename>"
	exit 1
fi

ARCHIVENAME=$1

rm -rf staging
mkdir -p staging/$ARCHIVENAME/x86_64
mkdir -p staging/$ARCHIVENAME/x86
mkdir -p staging/$ARCHIVENAME/docs
cp blockattack_standalone_launcher staging/$ARCHIVENAME/blockattack
cp README.txt staging/$ARCHIVENAME/README
chmod +x staging/$ARCHIVENAME/blockattack

cd ../../..

docker build -f source/misc/docker/Dockerfile.Ubuntu12.04build_Standalone . -t blockattack_test

echo Copying to: $(pwd)/source/misc/standalone/staging/$ARCHIVENAME

docker run -it --rm -v $(pwd)/source/misc/standalone/staging/$ARCHIVENAME/:/output blockattack_test /bin/bash -c "cp /staging/blockattack-game/Game/blockattack /output/x86_64/ && \
cp /usr/local/lib/libSDL2-2.0.so.0 /output/x86_64/ && \
cp /usr/local/lib/libphysfs.so.1  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_mixer-2.0.so.0  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_ttf-2.0.so.0  /output/x86_64/ && \
cp /usr/lib/x86_64-linux-gnu/libfreetype.so.6  /output/x86_64/ && \
cp /lib/x86_64-linux-gnu/libpng12.so.0  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_image-2.0.so.0  /output/x86_64/"

docker run -it --rm -v $(pwd)/source/misc/standalone/staging/$ARCHIVENAME/:/output blockattack_test /bin/bash -c "cp -r /staging/blockattack-game/source/misc/translation/locale /output/ && \
cp /staging/blockattack-game/Game/blockattack.data /output/ && \
cp -r /staging/blockattack-game/source/misc/icons /output/ && \
cp /staging/blockattack-game/COPYING /output/ && \
cp /staging/blockattack-game/man/blockattack.man /output/docs/ && \
cp /staging/blockattack-game/README.md /output/docs/README_ORG.md && \
chown -R 1000 /output"

scp source/misc/standalone/compile_requirements.sh blockattack_build:/
scp source/misc/standalone/install_requirements.sh blockattack_build:/
ssh blockattack_build /install_requirements.sh
ssh blockattack_build /compile_requirements.sh
ssh blockattack_build rm -rf /staging/blockattack-game
scp -r . blockattack_build:/staging/blockattack-game
ssh blockattack_build rm -f /staging/blockattack-game/CMakeCache.txt

ssh blockattack_build "BLOCKATTACK_VERSION=2.2.0-SNAPSHOT && cd /staging/blockattack-game && \
./packdata.sh && \
cmake -D Boost_USE_STATIC_LIBS=ON -D INSTALL_DATA_DIR=. -D CMAKE_INSTALL_PREFIX=. -D STANDALONE=1 . && make clean && make"

OUTPUT=$(pwd)/source/misc/standalone/staging/$ARCHIVENAME/

scp blockattack_build:/staging/blockattack-game/Game/blockattack $OUTPUT/x86/ && \
scp blockattack_build:/usr/local/lib/libSDL2-2.0.so.0 $OUTPUT/x86/ && \
scp blockattack_build:/usr/local/lib/libphysfs.so.1  $OUTPUT/x86/ && \
scp blockattack_build:/usr/local/lib/libSDL2_mixer-2.0.so.0  $OUTPUT/x86/ && \
scp blockattack_build:/usr/local/lib/libSDL2_ttf-2.0.so.0  $OUTPUT/x86/ && \
scp blockattack_build:/usr/lib/i386-linux-gnu/libfreetype.so.6 $OUTPUT/x86/ && \
scp blockattack_build:/lib/i386-linux-gnu/libpng12.so.0 $OUTPUT/x86/ && \
scp blockattack_build:/usr/local/lib/libSDL2_image-2.0.so.0  $OUTPUT/x86/

exit 0

docker build -f source/misc/docker/Dockerfile.Ubuntu12.04build_Standalone32 . -t blockattack_test

docker run -it --rm -v $(pwd)/source/misc/standalone/staging/$ARCHIVENAME/:/output blockattack_test /bin/bash -c "cp /staging/blockattack-game/Game/blockattack /output/x86/ && \
cp /usr/local/lib/libSDL2-2.0.so.0 /output/x86/ && \
cp /usr/local/lib/libphysfs.so.1  /output/x86/ && \
cp /usr/local/lib/libSDL2_mixer-2.0.so.0  /output/x86/ && \
cp /usr/local/lib/libSDL2_ttf-2.0.so.0  /output/x86/ && \
cp /usr/lib/i386-linux-gnu/libfreetype.so.6 /output/x86/ && \
cp /lib/i386-linux-gnu/libpng12.so.0 /output/x86/ && \
cp /usr/local/lib/libSDL2_image-2.0.so.0  /output/x86/"

cd source/misc/standalone/staging/
tar -cvjSf "$ARCHIVENAME.tar.bz2" "$ARCHIVENAME"
