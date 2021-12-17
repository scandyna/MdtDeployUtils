
This section explains the overview of various packages
and also how to build and install them.

Note: if you use Conan in your project,
you don't need to install MdtDeployUtils.

[[_TOC_]]

# Create a Conan package

The package version is picked up from git tag.
If working on MdtDeployUtils, go to the root of the source tree:
```bash
git tag x.y.z
conan create . scandyna/testing --profile $CONAN_PROFILE -s build_type=$BUILD_TYPE
```

To create a package without having a git tag:
```bash
conan create . x.y.z@scandyna/testing --profile $CONAN_PROFILE -s build_type=$BUILD_TYPE
```

Above examples will generate a package that uses the Qt version that is installed on the system,
or passed to the `CMAKE_PREFIX_PATH` of your build.

To create packages that depend on Conan Qt:
```bash
conan create . scandyna/testing -o MdtDeployUtils:use_conan_qt=True
```

# Conan packages

## MdtDeployUtils (tools)

Contains the `mdtdeployutils` executable,
the required shared libraries,
the CMake scripts for usage with CMake (i.e. CMake integration)
and the bash auto-completion script.

## MdtDeployUtilsCore

Contains the `MdtDeployUtils` core libraries, headers
and CMake files required to build projects using `MdtDeployUtilsCore`.

Depends on:
MdtCommandLineParser, MdtConsoleApplication, Qt5Core


# Debian packages

## mdtdeployutils-standalone (tools)

Contains the `mdtdeployutils` executable,
the required shared libraries,
the CMake scripts for usage with CMake (i.e. CMake integration)
and the bash auto-completion script.

## libmdt0deployutilscore

Contains the `MdtDeployUtils` core libraries.

Depends on:
libmdt0commandlineparser, libmdt0consoleapplication, libqt5core

## libmdt0deployutilscore-dev

Contains the headers and CMake files
required to build projects using `MdtDeployUtilsCore`.

Depends on:
libmdt0deployutilscore
