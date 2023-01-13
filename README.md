# libhal-starter

Starting project for libhal projects.

To learn how to build a project using `libhal-starter` please see the
[Guided Tutorial](https://libhal.github.io/tutorial/) from the libhal central
documentation.

The contents of libhal-starter are as follows:

```
libhal-starter
├── LICENSE
├── README.md
├── project
│   ├── CMakeLists.txt
│   ├── applications
│   │   ├── blinker.cpp
│   │   ├── button_and_led.cpp
│   │   └── hello_world.cpp
│   ├── arm_common.ld
│   ├── arm_toolchain.cmake
│   ├── conanfile.txt
│   ├── hardware_map.hpp
│   ├── main.cpp
│   ├── targets
│   │   ├── lpc4078
│   │   │   ├── initializer.cpp
│   │   │   ├── libhal.tweaks.hpp
│   │   │   └── lpc4078.ld
│   │   └── stm32f103
│   │       ├── initializer.cpp
│   │       ├── libhal.tweaks.hpp
│   │       └── stm32f103.ld
│   └── third_party
│       └── newlib.cpp
└── tests
    ├── CMakeLists.txt
    ├── conanfile.txt
    ├── libhal.tweaks.hpp
    ├── main.test.cpp
    ├── project.test.cpp
    └── run.sh
```

For details about how the project is laid out see the
[`project/README.md`](project/README.md)
file.

For details about unit testing a project and the test directory layout see
[`tests/README.md`](tests/README.md) file.
