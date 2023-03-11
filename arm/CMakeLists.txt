cmake_minimum_required(VERSION 3.20)

# Toolchain file which uses arm-none-eabi-gcc toolchain
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "Default to Debug Build")
endif(NOT DEFINED CMAKE_BUILD_TYPE)

# Set project name to demos
project(demos VERSION 0.0.1 LANGUAGES CXX)

# Import the liblpc40xx library (and all of its dependencies)
find_package(libhal-lpc40xx REQUIRED CONFIG)
find_package(libhal-rmd REQUIRED CONFIG)
find_package(libhal-esp8266 REQUIRED CONFIG)
find_package(libhal-util REQUIRED CONFIG)
find_package(libhal-pca REQUIRED CONFIG)

# List of demo applications
# To add a new demo to the list, simply add its filename without the .cpp
# extension to this list.
set(DEMOS pca-demo app app_serial app-wifi)

# List of targets in the LPC40 series MCU
set(TARGETS lpc4078)

# Iterate through each possible target and demo and generate a "make" target
# for each.
foreach(target IN LISTS TARGETS)
    foreach(demo IN LISTS DEMOS)
        set(current_project ${target}_${demo})
        message(STATUS "Generating Demo for \"${current_project}\"")
        add_executable(${current_project}
            main.cpp
            third_party/newlib.cpp
            targets/${target}/initializer.cpp
            applications/${demo}.cpp)
        target_include_directories(${current_project} PUBLIC
            ./
            targets/${target})
        target_compile_features(${current_project} PRIVATE cxx_std_20)

        target_link_options(${current_project} PRIVATE -u _printf_float)
        
        # TODO(#3): This will not work for libstm32 and other target libraries.
        target_link_libraries(${current_project} PRIVATE
            libhal::lpc4078
            libhal::rmd
            libhal::esp8266
            libhal::util
            libhal::pca)
        arm_post_build(${current_project})
    endforeach()
endforeach()
