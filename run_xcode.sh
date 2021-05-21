#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.llvm CMakeLists.txt

cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

mkdir xcode 2> /dev/null
cd xcode
rm CMakeCache.txt
cmake -G Xcode -T buildsystem=1 ..
cd ..
