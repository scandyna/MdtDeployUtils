from conans import ConanFile, CMake
import os

class MdtDeployUtilsCoreTest(ConanFile):
  settings = "os", "compiler", "build_type", "arch"
  generators = "cmake", "virtualenv"
  build_requires = "MdtCMakeModules/0.19.1@scandyna/testing"

  def configure_cmake(self):
    cmake = CMake(self)

    #cmake.definitions["SOME_DEF"] = "SOME_VALUE"

    return cmake

  def build(self):
      cmake = self.configure_cmake()
      cmake.configure()
      cmake.build()

  def test(self):
      cmake = self.configure_cmake()
      cmake.test(output_on_failure=True)
