#!/bin/sh

export CC="/usr/local/bin/clang"
export CXX="/usr/local/bin/clang++"
export LD="/usr/local/bin/ld.lld"
export AR="/usr/local/bin/llvm-ar"
export RANLIB="/usr/local/bin/llvm-ranlib"
export LDFLAGS="-L/usr/local/lib -Wl,-rpath,/usr/local/lib"
export CPPFLAGS="-I/usr/local/include/c++/v1 -I/usr/local/include"
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

mkdir llvm 2> /dev/null
cd llvm
rm CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=Release ..
cd ..
