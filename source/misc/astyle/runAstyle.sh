#! /bin/bash
astyle -t -j -y -c -k1 -z2 -A2 --pad-header ../../code/*.cpp 
astyle -t -j -y -c -k1 -z2 -A2 --pad-header ../../code/sago/*.cpp
astyle -t -j -y -c -k1 -z2 -A2 --pad-header ../../code/sago/*.hpp
