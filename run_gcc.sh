#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.gcc CMakeLists.txt
cd src
ln -sf CMakeLists.txt.gcc CMakeLists.txt
cd ..

source env_gcc.sh

rm -rf gcc 2> /dev/null
mkdir gcc 2> /dev/null
cd gcc
cmake -DCMAKE_OSX_SYSROOT="$(xcrun --show-sdk-path)" ..
