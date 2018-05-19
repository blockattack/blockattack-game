#! /bin/bash
mkdir -p template
xgettext -k_ ../../code/*.cpp ../../code/*.inc ../../code/*.hpp --output=template/blockattack_roftb.pot
