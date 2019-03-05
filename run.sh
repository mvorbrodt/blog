#!/bin/bash

function help {
	echo "$0 {a|ad|c|cd|l|ld|g|gd|x}"
	echo
	echo "    a - ALL"
	echo "   ad - ALL debug"
	echo "    c - Apple's clang"
	echo "   cd - Apple's clang debug"
	echo "    l - LLVM's clang"
	echo "   ld - LLVM's clang debug"
	echo "    g - GNU's gcc"
	echo "   gd - GNU's gcc debug"
	echo "    x - Cppcheck"
	echo
}

function check {
	echo "*************************************"
	echo "* cppcheck --std=c++14 --enable=all *"
	echo "*************************************"
	echo
	cppcheck \
		--std=c++14 \
		--enable=all \
		--platform=native \
		--suppress=unusedFunction \
		*.hpp *.cpp
	echo
}

function build_clang {
	echo "*************************************************"
	echo "* clang++ -Ofast -march=native -std=c++2a -lc++ *"
	echo "*************************************************"
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
	if [ -f playground ]; then
		./playground
		rm -f playground
		echo
	fi
}

function build_clang_debug {
	echo "**********************************************"
	echo "* clang++ -O0 -march=x86-64 -std=c++2a -lc++ *"
	echo "**********************************************"
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
	if [ -f playground ]; then
		./playground
		rm -f playground
		echo
	fi
}

function build_llvm {
	echo "***************************************************"
	echo "* clang++-7 -Ofast -march=native -std=c++2a -lc++ *"
	echo "***************************************************"
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
	if [ -f playground ]; then
		./playground
		rm -f playground
		echo
	fi
}

function build_llvm_debug {
	echo "************************************************"
	echo "* clang++-7 -O0 -march=x86-64 -std=c++2a -lc++ *"
	echo "************************************************"
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
	if [ -f playground ]; then
		./playground
		rm -f playground
		echo
	fi
}

function build_gcc {
	echo "**************************************************"
	echo "* g++-8 -Ofast -march=native -std=c++2a -lstdc++ *"
	echo "**************************************************"
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
	if [ -f playground ]; then
		./playground
		rm -f playground
		echo
	fi
}

function build_gcc_debug {
	echo "***********************************************"
	echo "* g++-8 -O0 -march=x86-64 -std=c++2a -lstdc++ *"
	echo "***********************************************"
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
	if [ -f playground ]; then
		./playground
		rm -f playground
		echo
	fi
}

if [ "$1" == 'c' ]; then
	build_clang
elif [ "$1" == 'cd' ]; then
	build_clang_debug
elif [ "$1" == 'l' ]; then
	build_llvm
elif [ "$1" == 'ld' ]; then
	build_llvm_debug
elif [ "$1" == 'g' ]; then
	build_gcc
elif [ "$1" == 'gd' ]; then
	build_gcc_debug
elif [ "$1" == 'a' ]; then
	build_clang
	build_llvm
	build_gcc
elif [ "$1" == 'ad' ]; then
	build_clang_debug
	build_llvm_debug
	build_gcc_debug
elif [ "$1" == 'x' ]; then
	check
else
	help
fi
