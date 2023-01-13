cmake_minimum_required(VERSION 3.20)

# Toolchain file which uses arm-none-eabi-gcc toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm_toolchain.cmake)

# Set project name to demos
project(demos VERSION 0.0.1 LANGUAGES CXX)

# In order for find_package to work, the prefix path must be set to the
# location of the generated conan package reside. In this case they will reside
# at the root of the "build" directory or the CMAKE_BINARY_DIR
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# Import the liblpc40xx library (and all of its dependencies)
find_package(liblpc40xx REQUIRED CONFIG)

# Set the path to the linekr scripts to the source directory or (..)
set(LINKER_SCRIPT_PATHS ${CMAKE_SOURCE_DIR}/)

# List of demo applications
# To add a new demo to the list, simply add its filename without the .cpp
# extension to this list.
set(DEMOS app)

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
        set_target_properties(${current_project} PROPERTIES CXX_EXTENSIONS OFF)
        target_compile_options(${current_project} PRIVATE ${CORTEX_M4F_FLAGS})
        target_link_options(${current_project} PRIVATE ${CORTEX_M4F_FLAGS}
            -L${LINKER_SCRIPT_PATHS}
            -T${LINKER_SCRIPT_PATHS}/targets/${target}/${target}.ld
            -Wl,-Map=${current_project}.map,--cref -Wl,--gc-sections
            -Wl,--print-memory-usage)

        # TODO(#3): This will not work for libstm32 and other target libraries.
        target_link_libraries(${current_project} PRIVATE
            liblpc40xx::liblpc40xx)
        arm_post_build(${current_project})
    endforeach()
endforeach()
