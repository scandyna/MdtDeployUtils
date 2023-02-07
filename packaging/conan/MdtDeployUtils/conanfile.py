from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
from conan.errors import ConanInvalidConfiguration
from conans import tools
import os


class MdtDeployUtilsConan(ConanFile):
  name = "MdtDeployUtils"
  license = "BSD 3-Clause"
  url = "https://gitlab.com/scandyna/mdtdeployutils"
  description = "Tools to help deploy C/C++ application binaries and their dependencies."
  settings = "os", "compiler", "build_type", "arch"
  #generators = "cmake", "cmake_paths", "virtualenv"
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
    # MdtDeployApplication uses MdtCMakeModules modules
    self.requires("MdtCMakeModules/0.19.3@scandyna/testing")


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
    self.tool_requires("boost/1.72.0", force_host_context=True)
    self.tool_requires("qt/5.15.6", force_host_context=True)
    self.tool_requires("MdtCommandLineParser/0.0.6@scandyna/testing", force_host_context=True)
    self.tool_requires("MdtConsoleApplication/0.4.5@scandyna/testing", force_host_context=True)


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


  def generate(self):
    tc = CMakeToolchain(self)
    tc.variables["FROM_CONAN_PROJECT_VERSION"] = self.version
    tc.variables["BUILD_APPS"] = "ON"
    tc.variables["INSTALL_CONAN_PACKAGE_FILES"] = "OFF"
    tc.variables["CMAKE_MESSAGE_LOG_LEVEL"] = "DEBUG"
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
    self.run("cmake --install . --config %s --component MdtDeployUtils_Runtime" % self.settings.build_type)


  def package_id(self):
    del self.info.settings.compiler
    del self.info.settings.build_type


  def package_info(self):
    self.cpp_info.set_property("cmake_find_mode", "none")
    self.cpp_info.builddirs.append( os.path.join("lib", "cmake", "Mdt0DeployUtils") )
    self.env_info.PATH.append( os.path.join(self.package_folder, "bin") )
