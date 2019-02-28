#!/bin/sh

echo "******************************"
echo "* CLANG -O0 -std=c++2a -lc++ *"
echo "******************************"
echo
rm -f playground
clang -O0 -std=c++2a -lc++ main.cpp -o playground
./playground
echo
