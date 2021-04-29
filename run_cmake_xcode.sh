#!/bin/sh

cd /Users/martin/Code/blog
ln -sf CMakeLists.txt.xcode CMakeLists.txt

cd src
ln -sf CMakeLists.txt.xcode CMakeLists.txt
cd ..

mkdir xcode
cd xcode
cmake -DCMAKE_TOOLCHAIN_FILE=/Users/martin/Code/vcpkg/scripts/buildsystems/vcpkg.cmake -G Xcode ..
cd ..
