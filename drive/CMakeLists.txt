cmake_minimum_required(VERSION 3.20)

project(demos VERSION 0.0.1 LANGUAGES CXX)

find_package(libhal-lpc40xx REQUIRED CONFIG)
find_package(libhal-rmd REQUIRED CONFIG)
find_package(libhal-esp8266 REQUIRED CONFIG)
find_package(libhal-util REQUIRED CONFIG)

set(DEMOS rmd-demo app magnet-demo)
set(TARGETS lpc4078_10mhz lpc4078_12mhz)

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
            libhal::rmd
            libhal::esp8266
            libhal::util)
        arm_post_build(${current_project})
    endforeach()
endforeach()
