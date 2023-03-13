cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE INTERNAL "Default to Debug Build")
endif(NOT DEFINED CMAKE_BUILD_TYPE)

project(demos VERSION 0.0.1 LANGUAGES CXX)

find_package(libhal-lpc REQUIRED CONFIG)
find_package(libhal-esp8266 REQUIRED CONFIG)
find_package(libhal-util REQUIRED CONFIG)
find_package(libhal-pca REQUIRED CONFIG)

set(DEMOS science)
set(TARGETS lpc4078)

add_link_options(-lc -lrdimon -u _printf_float)

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

        target_link_libraries(${current_project} PRIVATE
            libhal::lpc4078
            libhal::esp8266
            libhal::util
            libhal::pca)
        arm_post_build(${current_project})
    endforeach()
endforeach()
