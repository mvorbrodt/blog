#!/bin/sh

echo "**************************************************"
echo "* Apple CLANG -O0 -march=native -std=c++17 -lc++ *"
echo "**************************************************"
echo
rm -f playground
clang -O0 -march=native -std=c++17 -lc++ main.cpp -o playground
./playground
echo
