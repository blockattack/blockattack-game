#! /bin/bash
set -euo pipefail

TMPFILE=$(mktemp /tmp/sago-lint.XXXXXX)

find ./source/code/ -name "*.cpp" -not -path "./source/code/Libs/*" | xargs --no-run-if-empty grep --with-filename --line-number "^  " >> ${TMPFILE}  || true
find ./source/code/ -name "*.h*" -not -path "./source/code/Libs/*" | xargs --no-run-if-empty grep --with-filename --line-number "^  " >> ${TMPFILE}  || true

if [ -s "$TMPFILE" ]
then 
echo "In the following places the lines are indended with spaces. This project uses tabs for indentation."
echo "The rule applies to comments too although the check can be bypassed by using either C++ style comments or \" *\" to start a line (Doxygen comments)"
cat "$TMPFILE"
echo ""
echo "Validation failed"
rm "$TMPFILE"
exit 1
fi

rm "$TMPFILE"
