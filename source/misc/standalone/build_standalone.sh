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

docker build -f source/misc/docker/Dockerfile.Ubuntu14.04build_Standalone . -t blockattack_test

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

docker build -f source/misc/docker/Dockerfile.Ubuntu14.04build_Standalone32 . -t blockattack_test

docker run -it --rm -v $(pwd)/source/misc/standalone/staging/$ARCHIVENAME/:/output blockattack_test /bin/bash -c "cp /staging/blockattack-game/Game/blockattack /output/x86/ && \
cp /usr/local/lib/libSDL2-2.0.so.0 /output/x86/ && \
cp /usr/local/lib/libphysfs.so.1  /output/x86/ && \
cp /usr/local/lib/libSDL2_mixer-2.0.so.0  /output/x86/ && \
cp /usr/local/lib/libSDL2_ttf-2.0.so.0  /output/x86/ && \
cp /usr/local/lib/libSDL2_image-2.0.so.0  /output/x86/"

cd source/misc/standalone/staging/
tar -cvjSf "$ARCHIVENAME.tar.bz2" "$ARCHIVENAME"
