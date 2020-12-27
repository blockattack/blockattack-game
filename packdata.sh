#!/bin/bash
set -e
zip -9jX  Game/blockattack.data source/AUTH
pushd Game/data
zip -9X $(find ../blockattack.data * | sort) -x \*svn*
popd
pushd Game/mods/1.3.0.bricks_unpacked.data/
zip -9X ../1.3.0.bricks.data $(find * | sort)
popd
pushd man
gzip -9n -c blockattack.man > blockattack.6.gz
popd
pushd source/misc/translation/
./build_mo_all.sh
popd

exit 0
