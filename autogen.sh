#!/bin/bash
make clean
rm -rf bin/ CMakeCache.txt CMakeFiles/ cmake_install.cmake
cmake .
