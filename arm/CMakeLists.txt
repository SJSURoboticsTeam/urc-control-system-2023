cmake_minimum_required(VERSION 3.15)

# Set project name to demos
project(application LANGUAGES CXX)

libhal_build_demos(
    DEMOS
    application

    PACKAGES
    libhal-rmd
    libhal-soft
    libhal-esp8266
    libhal-mpu

    LINK_LIBRARIES
    libhal::rmd
    libhal::soft
    libhal::esp8266
    libhal::mpu

    SOURCES
    platform-implementations/esp8266_mission_control.cpp
)
