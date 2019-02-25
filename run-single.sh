#!/bin/sh

rm -f playground
clang -Ofast -march=native -std=c++17 -lc++ main.cpp -o playground
./playground
