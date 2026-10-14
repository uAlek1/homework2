#!/usr/bin/env bash
set -e
SRC="main.cpp test.cpp"
mkdir -p bin

g++     -O0 -std=c++20 $SRC -o bin/app-gcc
g++     -O3 -std=c++20 $SRC -o bin/app-gcc-o3
clang++ -O0 -std=c++20 $SRC -o bin/app-clang
clang++ -O3 -std=c++20 $SRC -o bin/app-clang-o3