#!/bin/sh

echo "*******************************"
echo "* G++ -O0 -std=c++2a -lstdc++ *"
echo "*******************************"
echo
rm -f playground
/usr/local/opt/gcc/bin/g++-8 -O0 -std=c++2a -L/usr/local/opt/gcc/lib/ -lstdc++ main.cpp -o playground
./playground
echo
