#!/bin/sh

echo "*********************************************"
echo "* G++ -O0 -march=native -std=c++17 -lstdc++ *"
echo "*********************************************"
echo
rm -f playground
/usr/local/opt/gcc/bin/g++-8 -O0 -std=c++17 -L/usr/local/opt/gcc/lib/ -lstdc++ main.cpp -o playground
./playground
echo
