#! /bin/bash
set -e

test $1

POFILE=$1

LANGUAGE=${POFILE##*/}
LANGUAGE=${LANGUAGE%%.*}
mkdir -p locale/$LANGUAGE/LC_MESSAGES
echo "Building $LANGUAGE from $POFILE"
msgfmt -o locale/$LANGUAGE/LC_MESSAGES/blockattack_roftb.mo $POFILE
