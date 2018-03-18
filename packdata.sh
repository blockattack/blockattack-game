#!/bin/bash
set -e
zip -9rjX  Game/blockattack.data source/AUTH
cd Game/data
zip -9rX ../blockattack.data * -x \*svn*
cd ../../man
gzip -9 -c blockattack.man > blockattack.6.gz
cd ../source/misc/translation/
./build_mo_all.sh

exit 0
