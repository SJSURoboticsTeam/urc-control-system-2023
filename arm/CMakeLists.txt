
cmake_minimum_required(VERSION 3.20)

project(arm_main.elf VERSION 0.0.1 LANGUAGES CXX)

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

add_executable(${PROJECT_NAME}
    main.cpp
    applications/application.cpp
    platforms/${platform}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_options(${PROJECT_NAME} PRIVATE
--oslib=semihost --crt0=minimal)
target_link_libraries(${PROJECT_NAME} PRIVATE
    libhal::${platform_library}
    libhal::util
    libhal::rmd
    libhal::soft
    libhal::esp8266
    libhal::mpu
    )
libhal_post_build(${PROJECT_NAME})
