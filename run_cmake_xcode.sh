#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.xcode CMakeLists.txt

cd src
ln -sf CMakeLists.txt.xcode CMakeLists.txt
cd ..

rm -rf xcode
mkdir xcode
cd xcode
cmake -G Xcode -T buildsystem=1 ..
cd ..
