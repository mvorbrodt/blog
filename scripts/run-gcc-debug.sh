#!/bin/sh

echo "*******************************"
echo "* G++ -O0 -std=c++2a -lstdc++ *"
echo "*******************************"
echo
rm -f playground
/usr/local/opt/gcc/bin/g++-8 -O0 -std=c++2a -I/usr/local/include -L/usr/local/opt/gcc/lib/ -L/usr/local/lib -lstdc++ -ltbb main.cpp -o playground
./playground
echo
