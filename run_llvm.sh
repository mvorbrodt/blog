#!/bin/sh

cd ~/Code/blog
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

source env_llvm.sh
export OPENSSL_ROOT_DIR=/usr/local/Cellar/openssl@3/3.0.1

rm -rf llvm 2> /dev/null
mkdir llvm 2> /dev/null
cd llvm
cmake ..
