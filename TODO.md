
# Tests

Add InstallSharedLibrariesTargetDendsOnTest to cmake/tests

In CI, add a test that executes on a machine without Qt installed
(should fail if Qt plugins not installed)

Use BUILD_REAL_INSTALL_TESTS

Also, make shure to run all the tests on the installed libraries
See Elf writer for why..

Re-enable 32-bit builds !!!
(See Elf / Pe for why)
The 2-3 cast warnings must then be fixed,
but there is a lot to test !!

# RPath

Add a option that fails when mdtdeployutils
has to move sections (in ELF file)
when the new RPath is to big.

See comments in README about why.

# Install

Remove RPATH for copied shared libraries

When targetting Linux system wide install,
like debian packages,
do not copy shared libraries mdtdeployutils depends on.

Maybe provide a option that can either:
 - Deploy mdtdeployutils with the dependencies
 - Install mdtdeployutils (use case: Debian package f.ex)
NOTE: above could also be valid for Conan package ?

For Debian packages, should provide a source package,
then use Debian tools to build.
Debian tooling to get dependencies may exists.
See what CPack DEB Generator can do.
NOTE: simply build with only Debian native pkgs
NOTE: hmm MdtApplication, MdtCommandLineParser ??
Should provide Debian packages for them,
and also Mdt0.

NOTE:
mdtdeployutils0-standalone
Debian package that installs allmost in /opt/ ?

Provides several components/packages:
- MdtDeployUtils          (Common usecase, so this is the intuitive name)
- MdtDeployUtilsLibs      (or -dev component)
- MdtDeployUtilsCMake

TODO: only MdtDeployUtils with components:
 - default: apps (Mdt_Runtime) + CMake modules + CMake package config files
 - dev (dev, headers, CMake package config files for headers etc..)


This version should be the winner:
```
${CMAKE_INSTALL_PREFIX}
  |
  |-bin
  |  |-mdtdeployutils   (Runtime)
  |
  |-include   (Dev)
  |
  |-lib
     |-libMdt0DeployUtilsCore.so  (Runtime)
     |-libDepA.so                 (Runtime)
     |-libDepB.so                 (Runtime)
     |-cmake
         |-Modules            (Runtime)
         |-Mdt0DeployUtils
         |   |-Mdt0DeployUtilsExecutable.cmake          (Runtime) adds mdtdeployutils IMPORTED TARGET
         |   |-Mdt0DeployUtilsModules.cmake             (Runtime)
         |   |-Mdt0DeployUtilsConfig.cmake              (Runtime)
         |   |-Mdt0DeployUtilsConfigVersion.cmake       (Runtime)
         |-Mdt0DeployUtilsCore
             |-Mdt0DeployUtilsCoreTargets.cmake         (Dev) adds libMdt0DeployUtilsCore.so IMPORTED TARGET
             |-Mdt0DeployUtilsCoreConfig.cmake          (Dev)
             |-Mdt0DeployUtilsCoreConfigVersion.cmake   (Dev)
```

Usage as tool:
```cmake
find_package(Mdt0 COMPONENTS DeployUtils REQUIRED)

add_executable(myApp myApp.cpp)
target_link_libraries(myApp PRIVATE Mdt0::PlainText Qt5::Widgets)

mdt_deploy_application(TARGET myApp)
```


Use MdtDeployUtils to create a app

${CMAKE_INSTALL_PREFIX}
  |
  |-include
  |
  |-lib
     |-libMdt0DeployUtils.so
     |-cmake
         |-Modules
         |-Mdt0DeployUtils
             |-Mdt0DeployUtilsConfig.cmake
             |-Mdt0DeployUtilsConfigVersion.cmake
         |-Mdt0DeployUtilsLibs
             |-Mdt0DeployUtilsLibsTargets.cmake
             |-Mdt0DeployUtilsLibsConfig.cmake
             |-Mdt0DeployUtilsLibsConfigVersion.cmake



# Conan file

settings should not allways depend on compiler etc..

See how to create a conanfile for a tool !
 --> See https://docs.conan.io/en/latest/devtools.html


# Command line tools

Provide stand-alone executables

Document README how to install them

Also, specify that they are not required to be installed
when using CMake API + conan

# CMake API

Function that finds required tools, like ldd:
mdt_find_deploy_utils_tools()

mdt_find_ldd()

mdt_find_deploy_utils_executables()

NOTE: above should be done by
find_package(Mdt0 COMPONENTS DeployUtils)

Option to not install libraries:
 - Comming from some paths
 - That have no RPATH set  <-- this is strange

mdt_get_shared_libraries_target_depends_on()

mdt_install_shared_libraries_target_depends_on()

find/install Qt plugins

mdt_install_qt5_platform_plugins()

find/install Mdt plugins

mdt_install_executable()

mdt_deploy_application()


# C++ code

See https://scandyna.gitlab.io/mdtdeployutils/cpp-api/html/todo.html

# CI

## "Install test"

- Build+test for UNIX system wide install (/usr)
- Build+test for NON Unix system wide install

See cmake/tests for the reason

Also install and run on a dicker image without Qt installed.

# Impl header files

Should be in a Impl sub-directory
(should not be suffixed _Impl)
Should not be installed

# tools executable

See how to link to Qt statically

Try also link statically to Mdt libs (conan option shared for those libs)

Document BUILD.md for conan options to link to static libs, like:
 - MdtApplication

# Licences

Command-line tools should be GPL

C++ library should be LGPL

CMake modules are BSD 3-Clause


# Problems common to all tools/APIs

Should be able to handle target architectures
that are different from the host one
(crosscompiling)

## Output results

Try a simple array on stdout
(errors are then put to stderr)

See limits on platforms/shells

See the doc of QProcess

Add option for array output format:
 - Space separated quoted strings:
   "/path/to/libA.so" "/path/to some/libB.so"
 - `;` separated strings:
   "/path/to/libA.so;/path/to some/libB.so"

Note: only those 2 formats, do not generalise sepearator option

--output-format  (no short option, -f is ambigous, force)

Maybe, option to provide a file:
--output-file "file"

## Auto-completion
