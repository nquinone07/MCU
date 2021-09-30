#!/bin/bash
echo "Converting from Windows -> UNIX Line Encoding..."
set -x
find . -type f | xargs -Ix sed -i.bak -r 's/\r//g' x
set +x
echo "Line encoding complete... Please check that all files were converted correctly. Backups were stored with .bak."
set -x
find . -type f -name '*.bak' | xargs -Ix rm x
set +x
