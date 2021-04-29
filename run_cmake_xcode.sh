#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.xcode CMakeLists.txt

cd src
ln -sf CMakeLists.txt.xcode CMakeLists.txt
cd ..

rm -rf xcode
mkdir xcode
cd xcode
cmake -DCMAKE_TOOLCHAIN_FILE=/Users/martin/Code/vcpkg/scripts/buildsystems/vcpkg.cmake -G Xcode ..
cd ..
