
This section explains the overview of various packages
and also how to build and install them.

Note: if you use Conan in your project,
you don't need to install MdtDeployUtils.

[[_TOC_]]


# Conan packages

To use the conan packages, add scandyna remote:
```bash
conan remote add scandyna https://gitlab.com/api/v4/projects/25668674/packages/conan
```

## MdtDeployUtils (tools)

Contains the `mdtdeployutils` executable,
the required shared libraries,
the CMake scripts for usage with CMake (i.e. CMake integration)
and the bash auto-completion script.

If you use MdtDeployUtils as a dependency in your project,
it will automatically be installed.
In that case, you should use
[virtualrunenv](https://docs.conan.io/en/latest/mastering/virtualenv.html?highlight=virtualrunenv).

It is also possible to install MdtDeployUtils with conan.
This will generate some files, so go to a location
where those generated files are acceptable:
```shell
conan install MdtDeployUtils/x.y.z@scandyna/testing -g virtualrunenv
source activate_run.sh
source bash-completion.sh
mdtdeployutils --help
```

## MdtDeployUtilsCore

Contains the `MdtDeployUtils` core libraries, headers
and CMake files required to build projects using `MdtDeployUtilsCore`.

Depends on:
MdtCommandLineParser, MdtConsoleApplication, Qt5Core


# Debian packages (not implemented yet)

## mdtdeployutils-standalone (tools)

Contains the `mdtdeployutils` executable,
the required shared libraries,
the CMake scripts for usage with CMake (i.e. CMake integration)
and the bash auto-completion script.

Status: not implemented, questionable
`mdtdeployutils` should be installed in some place
that is in the `PATH`, without the user having to do anything.

## mdtdeployutils (tools)

Contains the `mdtdeployutils` executable,
the CMake scripts for usage with CMake (i.e. CMake integration)
and the bash auto-completion script.

Depends on:
libmdt0deployutilscore

Status: not implemented
Will require a debian package repository
to also install dependencies in a somewhat easy way.

## libmdt0deployutilscore

Contains the `MdtDeployUtils` core libraries.

Depends on:
libmdt0commandlineparser, libmdt0consoleapplication, libqt5core

Status: not implemented
Same reasons as for mdtdeployutils

## libmdt0deployutilscore-dev

Contains the headers and CMake files
required to build projects using `MdtDeployUtilsCore`.

Depends on:
libmdt0deployutilscore

Status: not implemented
Same reasons as for mdtdeployutils


# Create a Conan package

The package version is picked up from git tag.
If working on MdtDeployUtils, go to the root of the source tree:
```bash
git tag x.y.z
conan create packaging/conan/MdtDeployUtils scandyna/testing --profile:build $CONAN_PROFILE_BUILD --profile:host $CONAN_PROFILE_HOST --settings:build build_type=Release --settings:host build_type=$BUILD_TYPE
```

To create a package without having a git tag:
```bash
conan create packaging/conan/MdtDeployUtils x.y.z@scandyna/testing --profile:build $CONAN_PROFILE_BUILD --profile:host $CONAN_PROFILE_HOST --settings:build build_type=Release --settings:host build_type=$BUILD_TYPE
```
