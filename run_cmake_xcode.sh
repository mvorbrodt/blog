#!/bin/sh

cd src
ln -sf CMakeLists.txt.xcode CMakeLists.txt
cd ..

mkdir xcode
cd xcode
cmake -G Xcode ..
