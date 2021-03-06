
This section explains how to build MdtDeployUtils
and run the unit tests.

[[_TOC_]]

# Required tools and libraries

The tools and libraries described in [README](README.md)
are also required here.

Additional required tools and libraries that can be managed by Conan:
 - [Catch2](https://github.com/catchorg/Catch2)

If you use Conan, those additional dependencies are not required to be installed explicitly.
Otherwise, see the documentation of the dependencies.

# Project configuration using Conan

Here are the available options:

| Option           | Default | Possible Values  | Explanations |
| -----------------|:------- |:----------------:|--------------|
| shared           | True    |  [True, False]   | Build as shared library |
| use_conan_boost  | False   |  [True, False]   | Use Boost as conan dependency |
| use_conan_qt     | False   |  [True, False]   | Use [conan Qt](https://github.com/bincrafters/conan-qt) as conan dependency |

## Using Conan profiles

When using Conan for dependency management,
it is recommended to use Conan profiles.
This permits to have personal binary repository,
avoiding to recompile everything everytime.
This becomes more important if Qt is managed by Conan.

This requires modifications in the `settings.yml` Conan configuration,
and also some profile files.
See my [conan-config repository](https://gitlab.com/scandyna/conan-config) for more informations.

Some following sections will rely on Conan profiles.

# Build MdtDeployUtils

Get MdtDeployUtils:
```bash
git clone git@gitlab.com:scandyna/_project_name_.git
```

Create a build directory and cd to it:
```bash
mkdir build
cd build
```

## Build on Linux with the native compiler

Install the dependencies:
```bash
conan install -s build_type=Debug --build=missing ..
```

Configure MdtDeployUtils:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
cmake-gui .
```

To build, run:
```cmd
make -j4
```

## Instrumented build with Gcc on Linux

Install the dependencies:
```bash
conan install --profile linux_gcc7_x86_64 -s build_type=RelWithDebInfo --build=missing ..
```

Configure MdtDeployUtils:
```bash
cmake -DCMAKE_BUILD_TYPE=Instrumented -DBUILD_TESTS=ON -DSANITIZER_ENABLE_ADDRESS=ON -DSANITIZER_ENABLE_UNDEFINED=ON ..
cmake-gui .
```

To build, run:
```cmd
make -j4
```

## Build on Linux with Clang and libc++

Install the dependencies:
```bash
conan install --profile linux_clang6.0_x86_64_libc++_qt_widgets_modules -s build_type=Debug -o MdtDeployUtils:use_conan_qt=True --build=missing ..
```

Activate the build environment:
```bash
source activate.sh
```

Configure MdtDeployUtils:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..
cmake-gui .
```

Build and run the tests:
```bash
cmake --build .
ctest . --output-on-failure
```

To restore the standard environment:
```bash
source deactivate.sh
```
