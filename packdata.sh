#!/bin/bash
set -e
zip -9rj Game/blockattack.data source/AUTH
cd Game/data
zip -9r ../blockattack.data * -x \*svn*

exit 0
