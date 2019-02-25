#!/bin/sh

rm -f playground
clang -O0 -march=native -std=c++17 -lc++ main.cpp -o playground
./playground
