#!/bin/sh

export CC="/usr/local/bin/clang"
export CXX="/usr/local/bin/clang++"
export LD="/usr/local/bin/ld64.lld"
export AR="/usr/local/bin/llvm-ar"
export RANLIB="/usr/local/bin/llvm-ranlib"

export CPPFLAGS="-I$(xcrun --show-sdk-path)/usr/include -I/usr/local/include/c++/v1 -I/usr/local/include"
export CXXFLAGS="$CPPFLAGS"
export LDFLAGS="-L/usr/local/lib -Wl,-rpath,/usr/local/lib"

alias cc=$CC
alias c++=$CXX
alias ld=$LD
alias ar=$AR
alias ranlib=$RANLIB
