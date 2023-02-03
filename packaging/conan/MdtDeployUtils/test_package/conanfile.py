from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
import os


class MdtDeployUtilsTest(ConanFile):
  settings = "os", "compiler", "build_type", "arch"
  generators = "CMakeDeps"
  #build_requires = "MdtCMakeModules/[0.16.0]@scandyna/testing"


  # In the package, dependencies are build requirements.
  # So, we don't inherit Qt in this test package
  def requirements(self):
    self.requires("qt/5.15.6")


  def generate(self):
    tc = CMakeToolchain(self)
    tc.variables["CMAKE_MESSAGE_LOG_LEVEL"] = "DEBUG"
    tc.generate()


  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()
    cmake.install()


  def test(self):
    cmake = CMake(self)
    cmake.test()
    # This does not work with new CMake tool
    #cmake.test(output_on_failure=True)
