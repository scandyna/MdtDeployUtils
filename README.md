
Tools to help deploy C/C++ application binaries and their dependencies.

[![pipeline status](https://gitlab.com/scandyna/mdtdeployutils/badges/experimental/pipeline.svg)](https://gitlab.com/scandyna/mdtdeployutils/-/pipelines/latest)


[[_TOC_]]

# Introduction

While developing an application, or a library, dependencies can be managed by a package manager, like [Conan](https://docs.conan.io).

To run tests that depends on shared libraries, some runtime environment can be necessary.
For more about this, see [the MdtRuntimeEnvironment CMake module](https://scandyna.gitlab.io/mdt-cmake-modules/Modules/MdtRuntimeEnvironment.html),
and also [the MdtAddTest CMake helper module](https://scandyna.gitlab.io/mdt-cmake-modules/Modules/MdtAddTest.html).

At some point, an application has to be deployed.
This requires to install at least an executable and its shared libraries.

To deploy Qt applications, some tools are available:
- [linuxdeployqt](https://github.com/probonopd/linuxdeployqt)
- [windeployqt](https://doc.qt.io/qt-6/windows-deployment.html)
- [macdeployqt](https://doc.qt.io/qt-5/macos-deployment.html)



# Usage

## CMake API

This section explains how to use MdtDeployUtils
to install a application and its dependencies.

Example `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyApp)

find_package(MdtCMakeModules REQUIRED)
find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Mdt0 COMPONENTS PlainText REQUIRED)
find_package(Mdt0 COMPONENTS DeployUtils REQUIRED)

add_executable(myApp myApp.cpp)
target_link_libraries(myApp PRIVATE Mdt0::PlainText Qt5::Widgets)

include(GNUInstallDirs)
include(MdtInstallDirs)
include(MdtDeployApplication)

mdt_deploy_application(
  TARGET myApp
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
)
```

If Conan is used, the `conanfile.txt` could look like:
```txt
[build_requires]
MdtCMakeModules/x.y.z@scandyna/testing
MdtDeployUtils/x.y.z@scandyna/testing

[generators]
CMakeDeps
CMakeToolchain
VirtualBuildEnv
```

Also add scandyna remote:
```bash
conan remote add scandyna https://gitlab.com/api/v4/projects/25668674/packages/conan
```

For the available CMake modules, and their usage,
see [the CMake API documentation](https://scandyna.gitlab.io/mdtdeployutils/cmake-api)

## Command line tools

The command-line tool, `mdtdeployutils`, should be installed first.
On Linux, it also supports Bash completion.

See [INSTALL](INSTALL.md).


The tools have to find some dependencies, like shared libraries, in some way.
One possibility is to give a list of paths to the command-line.
A other one is to set a custom `PATH`, sometimes also required to build the code.
Some details are available in the [C++ documentation](https://scandyna.gitlab.io/mdtdeployutils/cpp-api/libs_DeployUtils.html).

### Deploy a application

If you call `mdtdeployutils` from a build environment
which has every dependencies in the `PATH`,
this should work:
```bash
mdtdeployutils deploy-application "path/to/some/executable" "path/to/destination/directory"
```

Setting a build environment is not allways required,
in which case some more informations have to be passed:
```bash
mdtdeployutils deploy-application --search-prefix-path-list "C:/Qt/5.14.2/msvc2017_64,C:/.conan/1234df/1" --compiler-location vc-install-dir="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC" "path/to/some/executable" "path/to/destination/directory"
```

For more details:
```bash
mdtdeployutils deploy-application --help
```

Note that this tool will not generate, neither install, any CMake files.
If the application to deploy should be usable by CMake,
considere the [MdtDeployApplication CMake module](https://scandyna.gitlab.io/mdtdeployutils/cmake-api/Modules/MdtDeployApplication.html).

### Helpers for shared libraries

Copy shared libraries a target depends on:
```bash
mdtdeployutils copy-shared-libraries-target-depends-on "path/to/some/executable" "path/to/destination/directory"
```

### Other commands

Get a list of shared libraries a target depends on:
```bash
mdtdeployutils get-shared-libraries-target-depends-on "path/to/some/executable"
```
Status: not implemented.
Should see if it is usefull, and also how the list should be returned
(maybe a file ?).

Install a executable:
```bash
mdtdeployutils install-executable "someExe"
```
Status: not implemented. Probably not usefull.


## C++ API

This section will explain how to build your project using the MdtDeployUtils C++ API
with CMake and Conan.
This means that your project is itself probably a deployment utility using MdtDeployUtils.

For the available classes, functions, and their usage,
see [the C++ API documentation](https://scandyna.gitlab.io/mdtdeployutils/cpp-api)


### Required tools and libraries

Some tools and libraries are required to use the MdtDeployUtils C++ API:
 - Git
 - CMake
 - Conan (optional)
 - A compiler (Gcc or Clang or MSVC)
 - Make (optional)
 - Qt5
 - Sphinx (optional, only to build the documentation)
 - Doxygen (optional, only to build the documentation)

For a overview how to install them, see https://gitlab.com/scandyna/build-and-install-cpp

Required tools and libraries that can be managed by Conan:
 - [mdt-cmake-modules](https://gitlab.com/scandyna/mdt-cmake-modules)
 - Qt


If you use Conan, MdtDeployUtils and its dependencies that are managed by Conan
are not required to be installed explicitly.
Otherwise see [INSTALL](INSTALL.md).

### conanfile.txt

Create (or update) `conanfile.txt`:
```txt
[requires]
MdtDeployUtilsCore/x.y.z@scandyna/testing

[build_requires]
MdtCMakeModules/x.y.z@scandyna/testing

[generators]
CMakeDeps
CMakeToolchain
VirtualBuildEnv
```

### CMake project description

Update your CMakeLists.txt to use the required libraries:
```cmake
cmake_minimum_required(VERSION 3.15)
project(MyApp)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Mdt0 COMPONENTS DeployUtilsCore REQUIRED)

add_executable(myTool myTool.cpp)
target_link_libraries(myTool PRIVATE Mdt0::DeployUtilsCore Qt5::Gui)
```

### Build your project using MdtDeployUtils

Create a build directory and cd to it:
```bash
mkdir build
cd build
```

Install the dependencies:
```bash
conan install --profile:build $CONAN_PROFILE_BUILD --profile:host $CONAN_PROFILE_HOST --settings:build build_type=Release --settings:host build_type=Debug --build=missing ..
```
For more informations about conan profiles, see [BUILD](BUILD.md).

If you don't use the native compiler,
and your Conan profile defines one
(or it defines some other environments),
bring the required environment to the current shell:
```bash
source conanbuild.sh
```
On Windows:
```bash
.\conanbuild.bat
```

Configure your project:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake-gui .
```

Build your project:
```bash
cmake --build . --config Debug
```

# Notes about RPath

## Context

On systems that support rpath,
the rpath informations will be changed during install.
For Linux shared libraries, it will probably be set to `$ORIGIN`.
More details are described in the
[MdtSharedLibrariesDepencyHelpers](https://scandyna.gitlab.io/mdtdeployutils/cmake-api/Modules/MdtSharedLibrariesDepencyHelpers.html)
module.

If the target (a shared library or a executable) was initially compiled with long enougth rpath informations,
the new rpath will become smaller, which does not change the binary file layout
(only leaves some null bytes in the file).

If the rpath informations becomes bigger, the file layout will change.

## Technical explanation

For ELF files (Unix binaries), the rpath informations are encoded in the `.dynstr` section.
Just after this section, a other section starts.
Shifting or moving most of the sections is far to complex for a tool like `MdtDeployUtils`,
only the linker of the compiler knows all the details required to write a working ELF file.

The solution is to move the `.dynstr` (and maybe the `.dynamic`) section to the end.
Because those sections need to be loaded into memory at runtime,
a load segment (`PT_LOAD`) that covers them must be added.

By adding a segment, the program header table will grow,
requiring to also move some sections to the end of the file.
Current implementation is able to do this,
and a set of tests shows that the resulting ELF still works.
Despite that, only a very small set of binaries have been
tested on Linux, based on GNU ld generated ELF files.

For more technical details, take a look at the source code, mainly:
- libs/DeployUtils_Core/src/Mdt/DeployUtils/Impl/Elf/FileWriterFile.h
- libs/DeployUtils_Core/tests/src/ElfFileWriterImplTest_Unix.cpp

## Recommandation

If you build from the source code of the target to deploy,
compile it with some rpath informations that is at least as long as the final rpath.

When using `CMake`, this will probably be the case
for targets that depends on shared libraries.

Notice: if the `INSTALL_RPATH` property is set for a target,
and this target has no dependencies to any shared libraries,
CMake will tell the linker to add fake rpath informations
of the correct size once the target is installed.
Example of a generated `link.txt` for gcc:
```
-rpath,:::::::
```
which will be replaced by `$ORIGIN` at install time by CMake,
which I find clever.

# Install MdtDeployUtils

To install MdtDeployUtils see [INSTALL](INSTALL.md).

# Work on MdtDeployUtils

To build MdtDeployUtils and run the unit tests, see [BUILD](BUILD.md).
