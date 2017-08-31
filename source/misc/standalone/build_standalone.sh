#! /bin/bash
set -e

rm -rf staging
mkdir -p staging/x86_64
cp blockattack_standalone_launcher staging/blockattack
chmod +x staging/blockattack

cd ../../..

docker build -f source/misc/docker/Dockerfile.Ubuntu14.04build_Standalone . -t blockattack_test

echo Copying to: $(pwd)/source/misc/standalone/staging/

docker run -it --rm -v $(pwd)/source/misc/standalone/staging/:/output blockattack_test /bin/bash -c "cp /staging/blockattack-game/Game/blockattack /output/x86_64/ && \
cp /staging/blockattack-game/Game/blockattack.data /output/ && \
cp /usr/local/lib/libSDL2-2.0.so.0 /output/x86_64/ && \
cp /usr/lib/x86_64-linux-gnu/libphysfs.so.1  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_mixer-2.0.so.0  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_ttf-2.0.so.0  /output/x86_64/ && \
cp /usr/lib/x86_64-linux-gnu/libfreetype.so.6  /output/x86_64/ && \
cp /lib/x86_64-linux-gnu/libpng12.so.0  /output/x86_64/ && \
cp /usr/local/lib/libSDL2_image-2.0.so.0  /output/x86_64/"
