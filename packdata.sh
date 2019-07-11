#!/bin/bash
set -e
zip -9jX  Game/blockattack.data source/AUTH
cd Game/data
zip -9X $(find ../blockattack.data * | sort) -x \*svn*
cd ../../man
gzip -9n -c blockattack.man > blockattack.6.gz
cd ../source/misc/translation/
./build_mo_all.sh

exit 0
