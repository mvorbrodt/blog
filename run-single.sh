#!/bin/sh

echo "*****************************************************"
echo "* Apple CLANG -Ofast -march=native -std=c++17 -lc++ *"
echo "*****************************************************"
echo
rm -f playground
clang -Ofast -march=native -std=c++17 -lc++ main.cpp -o playground
./playground
echo
