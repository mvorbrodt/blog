#!/bin/sh

echo "******************************"
echo "* CLANG -O0 -std=c++2a -lc++ *"
echo "******************************"
echo
rm -f playground
clang -O0 -std=c++2a -lc++ main.cpp -o playground
./playground
echo

echo "*****************************"
echo "* LLVM -O0 -std=c++2a -lc++ *"
echo "*****************************"
echo
rm -f playground
/usr/local/opt/llvm/bin/clang -O0 -std=c++2a -L/usr/local/opt/llvm/lib -lc++ main.cpp -o playground
./playground
echo

echo "*******************************"
echo "* GCC -O0 -std=c++2a -lstdc++ *"
echo "*******************************"
echo
rm -f playground
/usr/local/opt/gcc/bin/g++-8 -O0 -std=c++2a -L/usr/local/opt/gcc/lib/ -lstdc++ main.cpp -o playground
./playground
echo