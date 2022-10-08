
[[_TOC_]]

# Build MdtDeployUtils

This section explains how to build MdtDeployUtils
and run the unit tests.

## Required tools and libraries

Some tools are required to build MdtDeployUtils:
 - Git
 - Conan
 - CMake
 - Make (can be other, like Ninja)
 - C++ compiler (like Gcc or Clang or MSVC)

Additional tools are required to generate the documentation:
 - Doxygen
 - Sphinx

For a overview how to install them, see https://gitlab.com/scandyna/build-and-install-cpp

Tools and libraries that are managed by Conan:
 - [Catch2](https://github.com/catchorg/Catch2)
 - Qt
 - [MdtCMakeModules](https://gitlab.com/scandyna/mdt-cmake-modules)
 - [MdtCMakeConfig](https://gitlab.com/scandyna/mdtcmakeconfig)


## Get the source code

Get the sources:
```bash
git clone https://gitlab.com/scandyna/mdtdeployutils.git
```

Create a build directory and cd to it:
```bash
mkdir build
cd build
```

## Install the dependencies

The recommended way is to use Conan to install the dependencies.
The examples are based on that.

Note: there is a `conanfile.py` at the root of the project tree.
This file is only used to install dependencies to work on MdtDeployUtils.
To create Conan packages, see [INSTALL](INSTALL.md).

### Conan options

Here are the available options:

| Option           | Default | Possible Values  | Explanations |
| -----------------|:------- |:----------------:|--------------|
| shared           | True    |  [True, False]   | Build as shared library |

### Using Conan profiles

When using Conan for dependency management,
it is recommended to use Conan profiles.
This permits to have personal binary repository,
avoiding to recompile everything everytime.
This becomes more important for dependencies like Qt.

This requires modifications in the `settings.yml` Conan configuration,
and also some profile files.
See my [conan-config repository](https://gitlab.com/scandyna/conan-config) for more informations.

Some following sections will rely on Conan profiles.

### Note about conan and new generators

Below examples are based on the new generators,
like CMakeDeps, CMakeToolchain and VirtualBuildEnv.

Some of those recommend to specify the build and host profile:
```bash
conan install --profile:build $CONAN_PROFILE_BUILD --profile:host $CONAN_PROFILE_HOST -s build_type=$BUILD_TYPE --build=missing ..
```

Some packages have build requirements.
Those build requirements don't have to be build in Debug mode for example.
The same is true for some options.
To avoid building those build requirements unnecessarily,
specify a build profile that has more chances to match [Conan center](https://conan.io/center)'s available packages,
as well as a Release build:
```bash
conan install --profile:build $CONAN_PROFILE_BUILD --profile:host $CONAN_PROFILE_HOST --settings:build build_type=Release --settings:host build_type=$BUILD_TYPE --options:host shared=$BUILD_SHARED_LIBS --build=missing ..
```

The new CMakeToolchain generator creates a `conan_toolchain.cmake` which will be passed to CMake:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ...
```
This will set some flags, like `CMAKE_CXX_FLAGS_INIT`.

Note: the compiler (like `CMAKE_CXX_COMPILER`) is not set in the generated toolchain file.
See also: https://github.com/conan-io/conan/issues/9962

The compiler is specified in the profiles, in a new (undocumented ?) buildenv section:
```conan
[buildenv]
CC=gcc-8
CXX=g++-8
```

To build (or at least generate the build system) using the correct compiler,
we have to get this information.
Here comes the conan VirtualBuildEnv, which generates somes scripts, like `conanbuild.sh`.

On UNIX like system, the workflow looks like:
```bash
conan install ...
source conanbuild.sh
cmake ...
source deactivate_conanbuild.sh
```
and on Windows:
```bash
conan install ...
.\conanbuild
cmake ...
.\deactivate_conanbuild
```

## Configure MdtDeployUtils

This is a example on Linux using gcc.

Install the dependencies:
```bash
conan install --profile:build linux_gcc8_x86_64 --profile:host linux_ubuntu-18.04_gcc8_x86_64_qt_widgets_modules_boost --settings:build build_type=Release --settings:host build_type=Debug --build=missing ..
```

Configure MdtDeployUtils:
```bash
source conanbuild.sh
cmake -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug ..
cmake-gui .
```

## Build and run the tests

Those examples use cmake to run the build,
which should work everywhere.

Build:
```bash
cmake --build . --config Debug -j4
```

To run the tests:
```bash
ctest . --output-on-failure -C Debug -j4
```

## Restore your environment

On Unix:
```bash
source deactivate_conanbuild.sh
```
On Windows:
```bash
.\deactivate_conanbuild
```
