#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.llvm CMakeLists.txt

cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

export OPENSSL_ROOT_DIR=/usr/local/Cellar/openssl@3/3.0.1

mkdir xcode 2> /dev/null
cd xcode
rm CMakeCache.txt 2> /dev/null
cmake -G Xcode -T buildsystem=1 ..
cd ..
