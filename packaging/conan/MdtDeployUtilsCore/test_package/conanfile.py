from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
import os

class MdtDeployUtilsCoreTest(ConanFile):
  settings = "os", "compiler", "build_type", "arch"
  generators = "CMakeDeps"


  def build_requirements(self):
    self.tool_requires("MdtCMakeModules/0.19.2@scandyna/testing", force_host_context=True)


  def generate(self):
    tc = CMakeToolchain(self)
    tc.variables["CMAKE_MESSAGE_LOG_LEVEL"] = "DEBUG"
    tc.generate()

  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()

  def test(self):
    cmake = CMake(self)
    cmake.test()
    # This does not work with new CMake tool
    #cmake.test(output_on_failure=True)
