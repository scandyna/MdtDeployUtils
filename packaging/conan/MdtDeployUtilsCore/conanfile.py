from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
from conan.errors import ConanInvalidConfiguration
from conans import tools
import os


class MdtDeployUtilsCoreConan(ConanFile):
  name = "MdtDeployUtilsCore"
  license = "BSD 3-Clause"
  url = "https://gitlab.com/scandyna/mdtdeployutils"
  description = "Library to create tools to help deploy C/C++ application binaries and their dependencies."
  # TODO: see TODO.md
  settings = "os", "compiler", "build_type", "arch"
  options = {"shared": [True, False]}
  default_options = {"shared": True}
  generators = "CMakeDeps", "VirtualBuildEnv"

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


  def requirements(self):
    self.requires("MdtCMakeConfig/0.0.5@scandyna/testing")
    self.requires("qt/5.15.6")
    #self.requires("MdtConsoleApplication/0.4.5@scandyna/testing")
    #self.requires("MdtCommandLineParser/0.0.6@scandyna/testing")


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
    self.tool_requires("MdtCMakeModules/0.19.2@scandyna/testing", force_host_context=True)
    self.tool_requires("boost/1.72.0", force_host_context=True)


  # The export exports_sources attributes does not work if the conanfile.py is in a sub-folder.
  # See https://github.com/conan-io/conan/issues/3635
  # and https://github.com/conan-io/conan/pull/2676
  # TODO: for libs, should only copy ../../../libs/DeployUtils_Core/src and CMakeLists.txt
  def export_sources(self):
    self.copy("*", src="../../../libs", dst="libs")
    self.copy("*", src="../../../cmake", dst="cmake")
    self.copy("MdtDeployUtilsConfig.cmake.in", src="../../../", dst=".")
    self.copy("CMakeLists.txt", src="../../../", dst=".")
    self.copy("LICENSE*", src="../../../", dst=".")


  def generate(self):
    tc = CMakeToolchain(self)
    #tc.variables["INSTALL_CONAN_PACKAGE_FILES"] = "ON"
    tc.variables["FROM_CONAN_PROJECT_VERSION"] = self.version
    tc.variables["BUILD_APPS"] = "OFF"
    # TODO: should be conditional (not for Debug build). What about multi-config ? Also, seems to fail most of the time
    #tc.variables["BUILD_USE_IPO_LTO_IF_AVAILABLE"] = "ON"
    tc.generate()


  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()


  def package(self):
    #cmake = self.configure_cmake()
    #cmake.install()
    self.run("cmake --install . --config %s --component MdtDeployUtilsCore_Runtime" % self.settings.build_type)
    self.run("cmake --install . --config %s --component MdtDeployUtilsCore_Dev" % self.settings.build_type)


  # Use the default package_id()
  # https://docs.conan.io/en/latest/creating_packages/define_abi_compatibility.html#define-abi-compatibility


  def package_info(self):
    self.cpp_info.set_property("cmake_file_name", "Mdt0DeployUtilsCore")
    self.cpp_info.set_property("cmake_target_name", "Mdt0::DeployUtilsCore")
    self.cpp_info.requires = ["MdtCMakeConfig::MdtCMakeConfig", "qt::qtCore"]
    self.cpp_info.libs = ["Mdt0DeployUtilsCore"]
