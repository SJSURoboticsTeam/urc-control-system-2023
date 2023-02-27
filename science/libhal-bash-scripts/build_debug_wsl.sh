#!/bin/bash

# Build everything. Not sure if conan must be ran each time prob not

# clear old build files
mkdir build

rm -r build/*

cd build

conan install .. -s build_type=Debug --build=missing

cmake .. -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake

make -j

echo "Debug script finished"