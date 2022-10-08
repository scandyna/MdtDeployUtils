from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration
import os


class MdtCommandLineParserConan(ConanFile):
  name = "MdtCommandLineParser"
  license = "BSD 3-Clause"
  url = "https://gitlab.com/scandyna/mdtcommandlineparser"
  description = "Library to help to create a command-line parser in a C++ application."
  settings = "os", "compiler", "build_type", "arch"
  options = {"shared": [True, False]}
  default_options = {"shared": True}
  # TODO fix once issue solved
  # Due to a issue using GitLab Conan repository,
  # version ranges are not possible.
  # See https://gitlab.com/gitlab-org/gitlab/-/issues/333638
  # TODO: Catch and MdtApplication should only be required for tests, so not in this package
  #requires = "MdtApplication/0.3.5@scandyna/testing"
  build_requires = "MdtCMakeModules/0.17.0@scandyna/testing","Catch2/v2.13.7x@scandyna/testing","MdtApplication/0.3.5@scandyna/testing"
  generators = "cmake", "cmake_paths", "virtualenv"

  # If no_copy_source is False, conan copies sources to build directory and does in-source build,
  # resulting having build files installed in the package
  # See also: https://github.com/conan-io/conan/issues/350
  no_copy_source = True

  # See: https://docs.conan.io/en/latest/reference/conanfile/attributes.html#short-paths
  # Should only be enabled if building on Windows causes problems
  short_paths = True

  def set_version(self):
    if not self.version:
      if not os.path.exists(".git"):
        raise ConanInvalidConfiguration("could not get version from git tag.")
      git = tools.Git()
      self.version = "%s" % (git.get_tag())


  # The export exports_sources attributes does not work if the conanfile.py is in a sub-folder.
  # See https://github.com/conan-io/conan/issues/3635
  # and https://github.com/conan-io/conan/pull/2676
  def export_sources(self):
    self.copy("*", src="../../../libs", dst="libs")
    self.copy("CMakeLists.txt", src="../../../", dst=".")
    self.copy("LICENSE*", src="../../../", dst=".")
    # TODO: remove once library is out of MdtDeployUtils
    self.copy("MdtDeployUtilsConfig.cmake.in", src="../../../", dst=".")
    self.copy("*", src="../../../cmake", dst="cmake")

  def configure_cmake(self):
    cmake = CMake(self)
    cmake.definitions["FROM_CONAN_PROJECT_VERSION"] = self.version
    cmake.definitions["WARNING_AS_ERROR"] = "ON"
    # TODO: should be conditional (not for Debug build). What about multi-config ?
    cmake.definitions["BUILD_USE_IPO_LTO_IF_AVAILABLE"] = "ON"
    # TODO: remove once library is out of MdtDeployUtils
    cmake.definitions["BUILD_APPS"] = "OFF"

    return cmake


  def build(self):
    cmake = self.configure_cmake()
    cmake.configure()
    cmake.build()


  def package(self):
    #cmake = self.configure_cmake()
    #cmake.install()
    self.run("cmake --install . --config %s --component MdtCommandLineParser_Runtime" % self.settings.build_type)
    self.run("cmake --install . --config %s --component MdtCommandLineParser_Dev" % self.settings.build_type)
