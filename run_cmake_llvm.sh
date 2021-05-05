#!/bin/sh

export PATH="/usr/local/bin:$PATH"
export CC=/usr/local/bin/clang
export CXX=/usr/local/bin/clang++
export LD=/usr/local/bin/ld.lld
export AR=/usr/local/bin/llvm-ar
export RANLIB=/usr/local/bin/llvm-ranlib
export LDFLAGS="-L/usr/local/lib -Wl,-rpath,/usr/local/lib"
export CPPFLAGS="-I/usr/local/include -I/usr/local/include/c++/v1/"
export CXXFLAGS="$CPPFLAGS"

alias cc=$CC
alias c++=$CXX
alias ld=$LD
alias ar=$AR
alias ranlib=$RANLIB

cd ~/Code/blog
ln -sf CMakeLists.txt.llvm CMakeLists.txt

cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

rm -rf llvm
mkdir llvm
cd llvm
cmake ..
cd ..
