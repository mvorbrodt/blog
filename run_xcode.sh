#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.llvm CMakeLists.txt

cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

mkdir xcode 2> /dev/null
cd xcode
rm CMakeCache.txt 2> /dev/null
cmake -DCMAKE_OSX_SYSROOT="$(xcrun --show-sdk-path)" -G Xcode -T buildsystem=1 ..
cd ..
