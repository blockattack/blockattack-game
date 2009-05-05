#!/bin/sh
zip -9rj Game/blockattack.data source/AUTH
cd Game
zip -9r blockattack.data gfx -x \*svn*
zip -9r blockattack.data music -x \*svn*
zip -9r blockattack.data sound -x \*svn*

exit 0
