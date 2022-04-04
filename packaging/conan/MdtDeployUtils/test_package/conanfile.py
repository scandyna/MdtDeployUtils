from conans import ConanFile, CMake
import os

class MdtDeployUtilsTest(ConanFile):
  settings = "os", "compiler", "build_type", "arch"
  generators = "cmake", "virtualenv"
  #build_requires = "MdtCMakeModules/[0.16.0]@scandyna/testing"

  def configure_cmake(self):
    cmake = CMake(self)

    cmake.definitions["CMAKE_MESSAGE_LOG_LEVEL"] = "DEBUG"

    return cmake

  def build(self):
      cmake = self.configure_cmake()
      cmake.configure()
      cmake.build()
      cmake.install()

  def test(self):
      cmake = self.configure_cmake()
      cmake.test(output_on_failure=True)
