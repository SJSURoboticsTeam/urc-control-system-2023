#!/bin/bash

# Build everything. Conan only needs to run once.

# *******WARNING when you chose a application to run this script in, choose vscode terminal or it won't work correctly.

# clear old build files
rm -r build/*

cd build

conan install .. -s build_type=Debug

cmake -G  "Unix Makefiles" .. -D CMAKE_BUILD_TYPE=Debug

make -j

echo "Done building"