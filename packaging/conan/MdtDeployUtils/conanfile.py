from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration
import os


class MdtDeployUtilsConan(ConanFile):
  name = "MdtDeployUtils"
  license = "BSD 3-Clause"
  url = "https://gitlab.com/scandyna/mdtdeployutils"
  description = "Tools to help deploy C/C++ application binaries and their dependencies."
  # TODO: see TODO.md
  settings = "os","arch"
  options = {"shared": [True, False],
             "use_conan_boost": [True, False],
             "use_conan_qt": [True, False]}
  default_options = {"shared": True,
                     "use_conan_boost": False,
                     "use_conan_qt": False}
  # TODO fix once issue solved
  # Due to a issue using GitLab Conan repository,
  # version ranges are not possible.
  # See https://gitlab.com/gitlab-org/gitlab/-/issues/333638
  # TODO: is Catch required here ?
  build_requires = "MdtApplication/0.3.5@scandyna/testing","MdtCMakeModules/0.16.0@scandyna/testing","Catch2/v2.13.7x@scandyna/testing"
  generators = "cmake", "cmake_paths", "virtualenv"

  # If no_copy_source is False, conan copies sources to build directory and does in-source build,
  # resulting having build files installed in the package
  # See also: https://github.com/conan-io/conan/issues/350
  no_copy_source = True

  # See: https://docs.conan.io/en/latest/reference/conanfile/attributes.html#short-paths
  # Should only be enabled if building with MSVC on Windows causes problems
  short_paths = False

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
    self.copy("*", src="../../../apps", dst="apps")
    self.copy("*", src="../../../libs", dst="libs")
    self.copy("*", src="../../../cmake", dst="cmake")
    self.copy("MdtDeployUtilsConfig.cmake.in", src="../../../", dst=".")
    self.copy("CMakeLists.txt", src="../../../", dst=".")
    self.copy("LICENSE*", src="../../../", dst=".")

  def requirements(self):

    if self.options.use_conan_boost:
      self.requires("boost/[>=1.65.1]@conan/stable")

    # Building 5.14.x causes currently problems (8.04.2020)
    # As workaround, try fix a known version that we can build
    if self.options.use_conan_qt:
      self.requires("qt/5.14.2@bincrafters/stable")
      self.options["MdtApplication"].use_conan_qt = True


  def configure_cmake(self):
    cmake = CMake(self)
    cmake.definitions["FROM_CONAN_PROJECT_VERSION"] = self.version
    cmake.definitions["WARNING_AS_ERROR"] = "ON"
    cmake.definitions["BUILD_USE_IPO_LTO_IF_AVAILABLE"] = "ON"

    return cmake


  def build(self):
    cmake = self.configure_cmake()
    cmake.configure()
    cmake.build()


  # TODO: check component install ?
  def package(self):
    #cmake = self.configure_cmake()
    #cmake.install()
    self.run('cmake --install . --component MdtDeployUtils_Runtime')

  def package_info(self):
    self.env_info.PATH.append(os.path.join(self.package_folder, "bin"))
