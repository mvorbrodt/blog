#!/bin/sh

export PATH="/usr/local/opt/llvm/bin:$PATH"
export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++
export LD=/usr/local/opt/llvm/bin/ld.lld
export AR=/usr/local/opt/llvm/bin/llvm-ar
export RANLIB=/usr/local/opt/llvm/bin/llvm-ranlib
export LDFLAGS="-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib"
export CPPFLAGS="-I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/"
#alias cc=$CC
#alias c++=$CXX
#alias ld=$LD
#alias ar=$AR
#alias ranlib=$RANLIB

cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

mkdir llvm
cd llvm
cmake ..
cd ..

cd src
rm -f CMakeLists.txt
cd ..
