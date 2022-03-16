#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

source env_llvm.sh

rm -rf llvm 2> /dev/null
mkdir llvm 2> /dev/null
cd llvm
cmake -DCMAKE_OSX_SYSROOT="$(xcrun --show-sdk-path)" ..
