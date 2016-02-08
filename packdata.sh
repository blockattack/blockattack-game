#!/bin/bash
set -e
zip -9rj Game/blockattack.data source/AUTH
cd Game/data
zip -9r ../blockattack.data * -x \*svn*
cd ../../man
docbook-to-man blockattack.sgml|gzip -9 > blockattack.6.gz
cd ../source/misc/translation/
./build_mo_all.sh

exit 0
