# NOTE EVERYTHING IS ONLY DESIGNED TO COMPILE FOR LPC40XX, SHOULD ADDITIONAL TARGETS BE DESIRED
# PLEASE CREATE A TARGETS FOLDER WITHIN libhal_configurations AND ITERATE OVER EACH INITIALIZER AND TWEAKS

cmake_minimum_required(VERSION 3.20)

project(science-robot.elf VERSION 0.0.1 LANGUAGES CXX)

find_package(libhal-lpc40xx REQUIRED CONFIG)

add_executable(${PROJECT_NAME} main.cpp libhal_configuration/newlib.cpp libhal_configuration/initializer.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC libhal_configuration)
target_link_libraries(${PROJECT_NAME} PRIVATE libhal::lpc4078)
arm_cortex_post_build(${PROJECT_NAME})

