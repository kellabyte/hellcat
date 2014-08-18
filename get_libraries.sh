#!/bin/bash

# Get and compile Haywire.
cd lib
git clone https://github.com/kellabyte/Haywire.git
cd Haywire
./build.sh
cd ..

# Get and compile LMDB
git clone https://gitorious.org/mdb/mdb.git
cd mdb/libraries/liblmdb
make
cd ../../../
