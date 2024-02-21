from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.errors import ConanInvalidConfiguration


class application(ConanFile):
    settings = "compiler", "build_type", "os", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualBuildEnv"
    options = {"platform": ["ANY"]}
    default_options = {"platform": "unspecified"}

    def build_requirements(self):
        self.tool_requires("libhal-cmake-util/[^2.2.0]")

    def requirements(self):
        # Application requirements
        self.requires("libhal-util/[^3.0.0]")
        self.requires("libhal-rmd/[3.0.0]")
        self.requires("libhal-soft/[3.0.1]")
        self.requires("libhal-esp8266/[2.0.1]")
        self.requires("libhal-mpu/2.0.0")
        self.requires("libhal/[2.2.0]")
        self.requires("libhal-pca/2.0.0")

        # List of supported platforms
        if str(self.options.platform).startswith("lpc40"):
            self.requires("libhal-lpc40/[^2.0.0]")
        else:
            raise ConanInvalidConfiguration(
                f"The platform '{str(self.options.platform)}' is not"
                f"supported.")

    def layout(self):
        platform_directory = "build/" + str(self.options.platform)
        cmake_layout(self, build_folder=platform_directory)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
