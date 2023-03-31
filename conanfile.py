from conans import ConanFile, tools
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
  generators = "CMakeDeps", "CMakeToolchain", "VirtualBuildEnv"
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
    self.requires("MdtCMakeConfig/0.0.5@scandyna/testing")
    self.requires("MdtConsoleApplication/0.4.5@scandyna/testing")
    # TODO: see if required once CommandLineParser is removed (if used in tests only, move to build_requirements)
    #self.requires("MdtCommandLineArguments/0.4.4@scandyna/testing")
    self.requires("MdtCommandLineParser/0.0.6@scandyna/testing")
    self.requires("MdtExecutableFileCore/0.0.1@scandyna/testing")


  # When using --profile:build xx and --profile:host xx ,
  # the dependencies declared in build_requires and tool_requires
  # will not generate the required files.
  # see:
  # - https://github.com/conan-io/conan/issues/10272
  # - https://github.com/conan-io/conan/issues/9951
  def build_requirements(self):
    # TODO fix once issue solved
    # Due to a issue using GitLab Conan repository,
    # version ranges are not possible.
    # See https://gitlab.com/gitlab-org/gitlab/-/issues/333638
    self.tool_requires("catch2/2.13.9", force_host_context=True)
    self.tool_requires("MdtCMakeModules/0.19.3@scandyna/testing", force_host_context=True)
