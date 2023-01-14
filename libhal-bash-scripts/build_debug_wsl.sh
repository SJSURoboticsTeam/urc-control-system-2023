#!/bin/bash

# Build everything. Conan only needs to run once.

# clear old build files
rm -r build/*

cd build

conan install .. -s build_type=Debug

cmake .. -D CMAKE_BUILD_TYPE=Debug

make -j

echo "Done building"