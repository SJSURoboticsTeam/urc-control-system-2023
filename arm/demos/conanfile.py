# Copyright 2023 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
        self.requires("libhal-rmd/[^3.0.0]")
        self.requires("libhal-soft/[^3.0.1]")
        self.requires("libhal-esp8266/[^2.0.1]")
        self.requires("libhal/[^2.0.3]")
        self.requires("libhal-pca/2.0.0")

        # List of supported platforms
        if str(self.options.platform).startswith("lpc40"):
            self.requires("libhal-lpc40/[^2.1.6]")
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