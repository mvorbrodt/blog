#!/bin/sh

cd /Users/martin/Code/blog
ln -sf CMakeLists.txt.xcode CMakeLists.txt

cd src
ln -sf CMakeLists.txt.xcode CMakeLists.txt
cd ..

mkdir xcode
cd xcode
cmake -G Xcode ..
cd ..
