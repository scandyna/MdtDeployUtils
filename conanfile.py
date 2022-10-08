from conans import ConanFile, CMake, tools
import os

# NOTE: this recipe is only to install dependnecies.
#       to create packages, see packaging subfolder
class MdtDeployUtilsConan(ConanFile):
  name = "MdtDeployUtils"
  #version = "0.1"
  license = "BSD 3-Clause"
  url = "https://gitlab.com/scandyna/mdtdeployutils"
  description = "Tools to help deploy C/C++ application binaries and their dependencies."
  # TODO: see TODO.md
  settings = "os", "compiler", "build_type", "arch"
  options = {"shared": [True, False]}
  default_options = {"shared": True}
  # TODO fix once issue solved
  # Due to a issue using GitLab Conan repository,
  # version ranges are not possible.
  # See https://gitlab.com/gitlab-org/gitlab/-/issues/333638
  requires = "MdtApplication/0.3.5@scandyna/testing"
  build_requires = "MdtCMakeModules/0.17.0@scandyna/testing", "Catch2/v2.13.7x@scandyna/testing"
  generators = "cmake", "cmake_paths", "virtualenv", "CMakeDeps", "CMakeToolchain"
  exports_sources = "apps/*", "libs/*", "cmake/*", "MdtDeployUtilsConfig.cmake.in", "CMakeLists.txt", "conanfile.py", "LICENSE.txt", "COPYING", "COPYING.LESSER"
  # If no_copy_source is False, conan copies sources to build directory and does in-source build,
  # resulting having build files installed in the package
  # See also: https://github.com/conan-io/conan/issues/350
  no_copy_source = True

  # See: https://docs.conan.io/en/latest/reference/conanfile/attributes.html#short-paths
  # Should only be enabled if building with MSVC on Windows causes problems
  short_paths = False

  def requirements(self):
    self.requires("qt/5.15.6")
    self.requires("boost/1.72.0")
