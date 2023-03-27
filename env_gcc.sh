#!/bin/sh

export CC="/usr/local/bin/gcc-12"
export CXX="/usr/local/bin/gcc-12"
export LD="/usr/local/bin/ld64.lld"
export AR="/usr/local/bin/gcc-ar-12"
export RANLIB="/usr/local/bin/gcc-ranlib-12"

export CPPFLAGS="-I$(xcrun --show-sdk-path)/usr/include -I/usr/local/Cellar/gcc/12.2.0/include/c++/12 -I/usr/local/include"
export CXXFLAGS="$CPPFLAGS"
export LDFLAGS="-L/usr/local/Cellar/gcc/12.2.0/lib/gcc/12 -L/usr/local/lib -Wl,-rpath,/usr/local/lib"

alias cc=$CC
alias c++=$CXX
alias ld=$LD
alias ar=$AR
alias ranlib=$RANLIB
