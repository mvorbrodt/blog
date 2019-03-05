#!/bin/bash

function build_clang {
	echo "***********************************************"
	echo "* CLANG -Ofast -march=native -std=c++2a -lc++ *"
	echo "***********************************************"
	echo
	rm -f playground
	clang++ \
		-Ofast \
		-march=native \
		-std=c++2a \
		-lc++ \
		-ltbb \
		main.cpp \
		-o playground
	./playground
	echo
}

function build_clang_debug {
	echo "********************************************"
	echo "* CLANG -O0 -march=x86-64 -std=c++2a -lc++ *"
	echo "********************************************"
	echo
	rm -f playground
	clang++ \
		-O0 \
		-march=x86-64 \
		-std=c++2a \
		-lc++ \
		-ltbb \
		main.cpp \
		-o playground
	./playground
	echo
}

function build_llvm {
	echo "**********************************************"
	echo "* LLVM -Ofast -march=native -std=c++2a -lc++ *"
	echo "**********************************************"
	echo
	rm -f playground
	clang++-7 \
		-Ofast \
		-march=native\
		-std=c++2a \
		-L/usr/local/opt/llvm/lib \
		-lc++ \
		-ltbb \
		main.cpp \
		-o playground
	./playground
	echo
}

function build_llvm_debug {
	echo "*******************************************"
	echo "* LLVM -O0 -march=x86-64 -std=c++2a -lc++ *"
	echo "*******************************************"
	echo
	rm -f playground
	clang++-7 \
		-O0 \
		-march=x86-64 \
		-std=c++2a \
		-L/usr/local/opt/llvm/lib \
		-lc++ \
		-ltbb \
		main.cpp \
		-o playground
	./playground
	echo
}

function build_gcc {
	echo "************************************************"
	echo "* G++ -Ofast -march=native -std=c++2a -lstdc++ *"
	echo "************************************************"
	echo
	rm -f playground
	g++-8 \
		-Ofast \
		-march=native \
		-std=c++2a \
		-I/usr/local/include \
		-L/usr/local/opt/gcc/lib/ \
		-L/usr/local/lib \
		-lstdc++ \
		-ltbb \
		main.cpp \
		-o playground
	./playground
	echo
}

function build_gcc_debug {
	echo "*********************************************"
	echo "* G++ -O0 -march=x86-64 -std=c++2a -lstdc++ *"
	echo "*********************************************"
	echo
	rm -f playground
	g++-8 \
		-O0 \
		-march=x86-64 \
		-std=c++2a \
		-I/usr/local/include \
		-L/usr/local/opt/gcc/lib/ \
		-L/usr/local/lib \
		-lstdc++ \
		-ltbb \
		main.cpp \
		-o playground
	./playground
	echo
}

function help {
	echo "$0 {c|cd|l|ld|g|gd|a|ad}"
	echo
	echo "    c - CLANG (default)"
	echo "   cd - CLANG Debug"
	echo "    l - LLVM"
	echo "   ld - LLVM Debug"
	echo "    g - GCC"
	echo "   gd - GCC Debug"
	echo "    a - ALL"
	echo "   ad - ALL Debug"
	echo
}

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

if [ $param = 'c' ]
then
	build_clang
elif [ $param = 'cd' ]
then
	build_clang_debug
elif [ $param = 'l' ]
then
	build_llvm
elif [ $param = 'ld' ]
then
	build_llvm_debug
elif [ $param = 'g' ]
then
	build_gcc
elif [ $param = 'gd' ]
then
	build_gcc_debug
elif [ $param = 'a' ]
then
	build_clang
	build_llvm
	build_gcc
elif [ $param = 'ad' ]
then
	build_clang_debug
	build_llvm_debug
	build_gcc_debug
else
	help
fi
