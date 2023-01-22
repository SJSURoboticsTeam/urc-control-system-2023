# URC 2023

Important Commands

1. make build folder inside of arm or drive folder
2. change into build folder directory
3. run `conan install .. -s build_type=Debug  --build=missing`
4. run `cmake .. -DCMAKE_BUILD_TYPE=Debug`
5. run `make -j`
6. run `nxpprog --control --binary="lpc4078_app.bin" --device="/dev/ttyUSB0"` - will probably need to change out device or binary for your correct info
