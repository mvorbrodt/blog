#!/bin/sh

echo "*******************************************"
echo "* LLVM -O0 -march=native -std=c++17 -lc++ *"
echo "*******************************************"
echo
rm -f playground
/usr/local/opt/llvm/bin/clang -O0 -std=c++17 -L/usr/local/opt/llvm/lib -lc++ main.cpp -o playground
./playground
echo
