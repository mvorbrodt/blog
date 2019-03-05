#!/bin/bash

if [ $# -eq 0 ]
then
	param='c'
elif [ $# -eq 1 ]
then
	param=$1
elif [ $# -ge 2 ]
then
	param='help'
fi

rm -f playground

if [ $param = 'c' ]
then
	echo "***********************************************"
	echo "* CLANG -Ofast -march=native -std=c++2a -lc++ *"
	echo "***********************************************"
	echo
	clang++ -Ofast -march=native -std=c++2a -lc++ -ltbb main.cpp -o playground
elif [ $param = 'cd' ]
then
	echo "******************************"
	echo "* CLANG -O0 -std=c++2a -lc++ *"
	echo "******************************"
	echo
	clang++ -O0 -std=c++2a -lc++ -ltbb main.cpp -o playground
elif [ $param = 'l' ]
then
	echo "**********************************************"
	echo "* LLVM -Ofast -march=native -std=c++2a -lc++ *"
	echo "**********************************************"
	echo
	/usr/local/opt/llvm/bin/clang -Ofast -march=native -std=c++2a -L/usr/local/opt/llvm/lib -lc++ -ltbb main.cpp -o playground
elif [ $param = 'ld' ]
then
	echo "*****************************"
	echo "* LLVM -O0 -std=c++2a -lc++ *"
	echo "*****************************"
	echo
	/usr/local/opt/llvm/bin/clang -O0 -std=c++2a -L/usr/local/opt/llvm/lib -lc++ -ltbb main.cpp -o playground
elif [ $param = 'g' ]
then
	echo "************************************************"
	echo "* G++ -Ofast -march=native -std=c++2a -lstdc++ *"
	echo "************************************************"
	echo
	/usr/local/opt/gcc/bin/g++-8 -Ofast -march=native -std=c++2a -I/usr/local/include -L/usr/local/opt/gcc/lib/ -L/usr/local/lib -lstdc++ -ltbb main.cpp -o playground
elif [ $param = 'gd' ]
then
	echo "*******************************"
	echo "* G++ -O0 -std=c++2a -lstdc++ *"
	echo "*******************************"
	echo
	/usr/local/opt/gcc/bin/g++-8 -O0 -std=c++2a -I/usr/local/include -L/usr/local/opt/gcc/lib/ -L/usr/local/lib -lstdc++ -ltbb main.cpp -o playground
else
	echo "$0 {c|cd|l|ld|g|gd}"
	echo
	echo "   c  - CLANG (default)"
	echo "   cd - CLANG Debug"
	echo "   l  - LLVM"
	echo "   ld - LLVM Debug"
	echo "   g  - GCC"
	echo "   gd - GCC Debug"
	echo
	exit
fi

./playground
