#!/bin/sh

echo "*****************************************************"
echo "* Apple CLANG -Ofast -march=native -std=c++17 -lc++ *"
echo "*****************************************************"
echo
rm -f playground
clang -Ofast -march=native -std=c++17 -lc++ main.cpp -o playground
./playground
echo

echo "**********************************************"
echo "* LLVM -Ofast -march=native -std=c++17 -lc++ *"
echo "**********************************************"
echo
rm -f playground
/usr/local/opt/llvm/bin/clang -Ofast -march=native -std=c++17 -L/usr/local/opt/llvm/lib -lc++ main.cpp -o playground
./playground
echo

echo "************************************************"
echo "* G++ -Ofast -march=native -std=c++17 -lstdc++ *"
echo "************************************************"
echo
rm -f playground
/usr/local/opt/gcc/bin/g++-8 -Ofast -march=native -std=c++17 -L/usr/local/opt/gcc/lib/ -lstdc++ main.cpp -o playground
./playground
echo
