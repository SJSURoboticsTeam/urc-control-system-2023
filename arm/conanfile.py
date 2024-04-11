from conan import ConanFile


class demos(ConanFile):
    python_requires = "libhal-bootstrap/[^1.0.1]"
    python_requires_extend = "libhal-bootstrap.demo"

    def requirements(self):
        bootstrap = self.python_requires["libhal-bootstrap"]
        bootstrap.module.add_demo_requirements(self)
        self.requires("libhal-rmd/[^4.0.0]")
        self.requires("libhal-soft/[^4.0.0]")
        self.requires("libhal-esp8266/[^3.0.0]")
        self.requires("libhal-mpu/[^3.0.0]")