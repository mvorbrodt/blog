#!/bin/sh

echo "***********************************************"
echo "* CLANG -Ofast -march=native -std=c++2a -lc++ *"
echo "***********************************************"
echo
rm -f playground
clang -Ofast -march=native -std=c++2a -lc++ -ltbb main.cpp -o playground
./playground
echo
