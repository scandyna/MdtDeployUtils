
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
- [windeployqt](https://doc.qt.io/qt-5/windows-deployment.html)
- [macdeployqt](https://doc.qt.io/qt-5/macos-deployment.html)



# Usage

## CMake API

This section explains how to use MdtDeployUtils
to install a application and its dependencies.

Example `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.14)
project(MyApp)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  conan_basic_setup(NO_OUTPUT_DIRS)
endif()

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
MdtCMakeModules/[>=x.y.z]@scandyna/testing
MdtDeployUtils/[>=x.y.z]@scandyna/testing

[generators]
cmake
virtualenv
```

For the available CMake modules, and their usage,
see [the CMake API documentation](https://scandyna.gitlab.io/mdtdeployutils/cmake-api)

## Command line tools

The command-line tool, `mdtdeployutils`, should be installed first.
On Linux, it also supports Bash completion.

See [INSTALL](INSTALL.md).

To deploy a application:
```bash
mdtdeployutils deploy-application "appExecutable"
```




```bash
mdtdeployutils get-shared-libraries-target-depends-on "path/to/some/executable"

mdtdeployutils copy-shared-libraries-target-depends-on "path/to/some/executable" "path/to/destination/directory"

mdtdeployutils install-executable "someExe"
```

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

Qt5 could be managed by Conan using [conan-qt](https://github.com/bincrafters/conan-qt),
but this does not allways work,
and building Qt if the required binaries are not available can be long.
This is why a option to use a non Conan managed Qt is provided.

Required tools and libraries that can be managed by Conan:
 - [mdt-cmake-modules](https://gitlab.com/scandyna/mdt-cmake-modules)


If you use Conan, MdtDeployUtils and its dependencies that are managed by Conan
are not required to be installed explicitly.
Otherwise see [INSTALL](INSTALL.md).

### conanfile.txt

Create (or update) `conanfile.txt`:
```txt
[requires]
MdtDeployUtils/[>=x.y.z]@scandyna/testing

[build_requires]
MdtCMakeModules/[>=x.y.z]@scandyna/testing

[generators]
cmake
virtualenv
```

### CMake project description

Update your CMakeLists.txt to use the required libraries:
```cmake
cmake_minimum_required(VERSION 3.14)
project(MyApp)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  conan_basic_setup(NO_OUTPUT_DIRS)
endif()

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Mdt0 COMPONENTS DeployUtilsCore REQUIRED)

add_executable(myTool myTool.cpp)
target_link_libraries(myTool PRIVATE Mdt0::DeployUtilsCore Qt5::Gui)
```

### Build your project using MdtDeployUtils on Linux with the native compiler

Create a build directory and cd to it:
```bash
mkdir build
cd build
```

Install the dependencies:
```bash
conan install -s build_type=Release --build=missing ..
```

Configure your project:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake-gui .
```

Build your project:
```bash
make -j4
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
