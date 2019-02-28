#!/bin/sh

echo "************************************************"
echo "* G++ -Ofast -march=native -std=c++2a -lstdc++ *"
echo "************************************************"
echo
rm -f playground
/usr/local/opt/gcc/bin/g++-8 -Ofast -march=native -std=c++2a -L/usr/local/opt/gcc/lib/ -lstdc++ main.cpp -o playground
./playground
echo
