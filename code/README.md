# libhal-starter Project Directory

```
project
├── CMakeLists.txt
├── applications
│   ├── blinker.cpp
│   ├── button_and_led.cpp
│   └── hello_world.cpp
├── arm_common.ld
├── arm_toolchain.cmake
├── conanfile.txt
├── hardware_map.hpp
├── main.cpp
├── targets
│   ├── lpc4078
│   │   ├── initializer.cpp
│   │   ├── libhal.tweaks.hpp
│   │   └── lpc4078.ld
│   └── stm32f103
│       ├── initializer.cpp
│       ├── libhal.tweaks.hpp
│       └── stm32f103.ld
└── third_party
    └── newlib.cpp
```

## `applications/` 🟡

This directory holds single file application programs. Each file is a self
contained application.

## `conanfile.txt`

Standard Conan file with dependencies for the project.

## `CMakeLists.txt`

CMake is the default build system for libhal projects due to its wide adoption
and ubiquity. This is the script that is read by `cmake` in order to generate
make files and other artifacts for actually building the final binary. The file
is documented with comments to explain each part of the file. Take a look to
get an idea of what the building blocks are and how to use them.

If you wanted to add more applications to the starter, you could add an
additional `.cpp` file to the `applications/` directory and then add the name
of that file to the DEMOS list:

```cmake
set(DEMOS blinker button_and_led hello_world new_application)
```

## `arm_common.ld`

This file is boilerplate linker script code that can be configured through some
linker script variables. It only supports single flash and RAM memory banks,
but this works for most use cases. This file is also well documented, so if you
are interested in understanding linker scripts, you can take a look at this
file's contents as well.

## `arm_toolchain.cmake`

To develop software for an embedded system you'll need to use a cross compiler.
In order to integrate that cross compiler into CMake you will need to  use a
tool chain CMake file. This all includes all of the changes needed to integrate
the arm gcc cross compiler into CMake.
