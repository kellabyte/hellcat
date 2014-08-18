#!/bin/bash
mkdir build
#./clean.sh
cd build
cmake ..
make VERBOSE=1
