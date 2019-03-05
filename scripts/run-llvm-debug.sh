#!/bin/sh

echo "*****************************"
echo "* LLVM -O0 -std=c++2a -lc++ *"
echo "*****************************"
echo
rm -f playground
/usr/local/opt/llvm/bin/clang -O0 -std=c++2a -L/usr/local/opt/llvm/lib -lc++ -ltbb main.cpp -o playground
./playground
echo
