#! /bin/bash
set -e

rm -rf staging
mkdir staging

cd ../../..

docker build -f source/misc/docker/Dockerfile.Ubuntu14.04build . -t blockattack_test

echo Copying to: $(pwd)/source/misc/standalone/staging/

docker run -it --rm -v $(pwd)/source/misc/standalone/staging/:/output blockattack_test /bin/bash -c "cp /staging/blockattack-game/Game/blockattack /output/ && cp /staging/blockattack-game/Game/blockattack.data /output/"
