
cmake_minimum_required(VERSION 3.20)

project(arm_demo.elf VERSION 0.0.1 LANGUAGES CXX)

set(platform_library $ENV{LIBHAL_PLATFORM_LIBRARY})
set(platform $ENV{LIBHAL_PLATFORM})

if("${platform_library}" STREQUAL "")
    message(FATAL_ERROR
        "Build environment variable LIBHAL_PLATFORM_LIBRARY is required for " "this project.")
endif()

find_package(libhal-${platform_library} REQUIRED CONFIG)
find_package(libhal-util REQUIRED CONFIG)
find_package(libhal-rmd REQUIRED CONFIG)
find_package(libhal-soft REQUIRED CONFIG)
find_package(libhal-esp8266 REQUIRED CONFIG)
find_package(libhal-mpu REQUIRED CONFIG)

add_executable(arm_angle.elf
    main.cpp
    applications/arm_angle.cpp
    ../platform-implementations/rmd_mcx_configurer.cpp
    platforms/${platform}.cpp)

add_executable(zero_motor.elf
    main.cpp
    applications/zero_motor.cpp
    ../platform-implementations/rmd_mcx_configurer.cpp
    platforms/${platform}.cpp)

target_include_directories(arm_angle.elf PUBLIC .)
target_compile_features(arm_angle.elf PRIVATE cxx_std_20)
target_link_options(arm_angle.elf PRIVATE
--oslib=semihost --crt0=minimal)
target_link_libraries(arm_angle.elf PRIVATE
    libhal::${platform_library}
    libhal::util
    libhal::rmd
    libhal::soft
    libhal::esp8266
    libhal::mpu
    )
libhal_post_build(arm_angle.elf)

target_include_directories(zero_motor.elf PUBLIC .)
target_compile_features(zero_motor.elf PRIVATE cxx_std_20)
target_link_options(zero_motor.elf PRIVATE
--oslib=semihost --crt0=minimal)
target_link_libraries(zero_motor.elf PRIVATE
    libhal::${platform_library}
    libhal::util
    libhal::rmd
    libhal::soft
    libhal::esp8266
    libhal::mpu
    )
libhal_post_build(zero_motor.elf)