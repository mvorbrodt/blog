#!/bin/sh

echo "******************************"
echo "* CLANG -O0 -std=c++17 -lc++ *"
echo "******************************"
echo
rm -f playground
clang -O0 -std=c++17 -lc++ main.cpp -o playground
./playground
echo
