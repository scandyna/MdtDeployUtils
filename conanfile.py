from conans import ConanFile, CMake, tools
import os


class MdtDeployUtilsConan(ConanFile):
  name = "MdtDeployUtils"
  #version = "0.1"
  license = "BSD 3-Clause"
  url = "https://gitlab.com/scandyna/mdtdeployutils"
  description = "Tools to help deploy C/C++ application binaries and their dependencies."
  # TODO: see TODO.md
  settings = "os", "compiler", "build_type", "arch"
  options = {"shared": [True, False],
             "use_conan_qt": [True, False],
             "build_tests": [True, False]}
  default_options = {"shared": True,
                     "use_conan_qt": False,
                     "build_tests": False}
  requires = "MdtCMakeModules/[0.14.12]@scandyna/testing"
  generators = "cmake", "cmake_paths", "virtualenv"
  exports_sources = "libs/*", "cmake/*", "CMakeLists.txt", "conanfile.py", "LICENSE.txt", "COPYING", "COPYING.LESSER"
  # If no_copy_source is False, conan copies sources to build directory and does in-source build,
  # resulting having build files installed in the package
  # See also: https://github.com/conan-io/conan/issues/350
  no_copy_source = True

  # See: https://docs.conan.io/en/latest/reference/conanfile/attributes.html#short-paths
  # Should only be enabled if building with MSVC on Windows causes problems
  short_paths = False

  def set_version(self):
    if os.path.exists(".git"):
      git = tools.Git()
      self.version = "%s" % (git.get_tag())


  def requirements(self):

    if self.options.build_tests:
      self.requires("Catch2/[>=2.11.1]@catchorg/stable")

    # Building 5.14.x causes currently problems (8.04.2020)
    # As workaround, try fix a known version that we can build
    if self.options.use_conan_qt:
      self.requires("qt/5.12.7@bincrafters/stable")


  def package_id(self):
    del self.info.options.build_tests


  def configure_cmake(self):
    cmake = CMake(self)
    cmake.definitions["FROM_CONAN_PROJECT_VERSION"] = self.version
    cmake.definitions["WARNING_AS_ERROR"] = "ON"

    if self.settings.compiler == "gcc" or self.settings.compiler == "clang":
      if self.settings.compiler.sanitizer == "Thread":
        cmake.definitions["SANITIZER_ENABLE_THREAD"] = "ON"

    return cmake


  def build(self):
    cmake = self.configure_cmake()
    cmake.configure()
    cmake.build()


  def package(self):
    cmake = self.configure_cmake()
    cmake.install()
