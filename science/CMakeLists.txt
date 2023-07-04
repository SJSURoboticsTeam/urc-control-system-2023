# Copyright 2023 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.20)

project(starter_project.elf VERSION 0.0.1 LANGUAGES CXX)

set(platform_library $ENV{LIBHAL_PLATFORM_LIBRARY})
set(platform $ENV{LIBHAL_PLATFORM})

if("${platform_library}" STREQUAL "")
    message(FATAL_ERROR
        "Build environment variable LIBHAL_PLATFORM_LIBRARY is required for " "this project.")
endif()

find_package(libhal-${platform_library} REQUIRED CONFIG)
find_package(libhal-util REQUIRED CONFIG)

add_executable(${PROJECT_NAME}
    main.cpp
    application.cpp
    platforms/${platform}.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE
    libhal::${platform_library}
    libhal::util)

libhal_post_build(${PROJECT_NAME})
