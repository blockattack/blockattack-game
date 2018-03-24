#! /bin/bash
mkdir -p template
xgettext -k_ ../../code/*.cpp ../../code/*.inc --output=template/blockattack_roftb.pot
