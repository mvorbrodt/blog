#!/bin/sh

# Old paths using brew's llvm
# export PATH="/usr/local/opt/llvm/bin:$PATH"
# export CC=/usr/local/opt/llvm/bin/clang
# export CXX=/usr/local/opt/llvm/bin/clang++
# export LD=/usr/local/opt/llvm/bin/ld.lld
# export AR=/usr/local/opt/llvm/bin/llvm-ar
# export RANLIB=/usr/local/opt/llvm/bin/llvm-ranlib
# export LDFLAGS="-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib"
# export CPPFLAGS="-I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/"
# export CXXFLAGS="$CPPFLAGS"

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

cd /Users/martin/Code/blog
ln -sf CMakeLists.txt.llvm CMakeLists.txt

cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..

mkdir llvm
cd llvm
cmake ..
cd ..
