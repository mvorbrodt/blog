#!/bin/sh

echo "**********************************************"
echo "* LLVM -Ofast -march=native -std=c++2a -lc++ *"
echo "**********************************************"
echo
rm -f playground
/usr/local/opt/llvm/bin/clang -Ofast -march=native -std=c++2a -L/usr/local/opt/llvm/lib -lc++ -ltbb main.cpp -o playground
./playground
echo
