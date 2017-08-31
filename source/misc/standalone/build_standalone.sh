#! /bin/bash
set -e

rm -rf staging
mkdir -p staging/x86_64
cp blockattack_standalone_launcher staging/blockattack
chmod +x staging/blockattack

cd ../../..

docker build -f source/misc/docker/Dockerfile.Ubuntu14.04build_Standalone . -t blockattack_test

echo Copying to: $(pwd)/source/misc/standalone/staging/

docker run -it --rm -v $(pwd)/source/misc/standalone/staging/:/output blockattack_test /bin/bash -c "cp /staging/blockattack-game/Game/blockattack /output/x86_64/ && cp /staging/blockattack-game/Game/blockattack.data /output/"
