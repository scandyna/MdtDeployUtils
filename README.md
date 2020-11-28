
Tools to help deploy C/C++ application binaries and their dependencies.

[[_TOC_]]

# TODO

Explain difference between handling dependencies for dev,
which Conan is great for,
and deploy a application with shared libs, plugins, etc...

# Usage

## Command line tools

```bash
mdtdeployutils get-shared-libraries-target-depends-on "path/to/some/executable"

mdtdeployutils copy-shared-libraries-target-depends-on "path/to/some/executable" "path/to/destination/directory"

mdtdeployutils install-executable "someExe"
```

## CMake API

For the available CMake modules, and their usage,
see [the CMake API documentation](https://scandyna.gitlab.io/mdtdeployutils/cmake-api)

## C++ API

For the available classes, functions, and their usage,
see [the C++ API documentation](https://scandyna.gitlab.io/mdtdeployutils/cpp-api)


This section will explain how to build your project using the MdtDeployUtils C++ API
with CMake and Conan.

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

[generators]
cmake
virtualenv
```

### CMake project description

Update your CMakeLists.txt to use the required libraries:
```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  conan_basic_setup(NO_OUTPUT_DIRS)
endif()

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Core Gui REQUIRED)
find_package(Mdt0 COMPONENTS _Project_Lib_Name_ REQUIRED)

add_executable(myApp myApp.cpp)
target_link_libraries(myApp Mdt0::_Project_Lib_Name_)
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

# Install _Project_name_

To install _Project_name_ see [INSTALL](INSTALL.md).

# Work on _Project_name_

To build _Project_name_ and run the unit tests, see [BUILD](BUILD.md).
