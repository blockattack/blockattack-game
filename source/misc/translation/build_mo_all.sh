#! /bin/bash
set -e

ls po/*.po | xargs -n1 ./build_mo_single_file.sh
