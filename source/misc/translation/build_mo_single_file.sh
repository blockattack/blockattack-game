#! /bin/bash
set -e

test $1

POFILE=$1

LANGUAGE=${1##*/}
LANGUAGE=${LANGUAGE%%_*}
mkdir -p locale/$LANGUAGE/LC_MESSAGES
echo "Building $LANGUAGE from $1"
msgfmt -o locale/$LANGUAGE/LC_MESSAGES/blockattack_roftb.mo $1
