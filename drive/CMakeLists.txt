cmake_minimum_required(VERSION 3.20)

project(drive LANGUAGES CXX)

libhal_build_demos(
  DEMOS
  application

  SOURCES
  platform-implementations/drc_speed_sensor.cpp
  platform-implementations/esp8266_mission_control.cpp
  # platform-implementations/serial_mission_control.cpp
  platform-implementations/offset_servo.cpp
  implementations/command_lerper.cpp
  implementations/mode_switcher.cpp
  implementations/tri_wheel_router.cpp

  PACKAGES
  libhal-rmd
  libhal-soft
  libhal-esp8266

  LINK_LIBRARIES
  libhal::rmd
  libhal::soft
  libhal::esp8266
)