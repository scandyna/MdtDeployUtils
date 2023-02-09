
# Introduction

This directory contains tests for the final tools
(``MdtDeployUtils`` including the ``mdtdeployutils`` executable
and integrations, like CMake).


# Structure

apps:

Some projects that are compiled then used by the tests.
Those projects are compiled at the same time than the project.


system:

Contains tests to check the final interfaces.
Those are generated during CMake configure/generate time,
then executed during test (ctest).

system/Cli:

Checks the CLI (command line interface).

Tests named ``System_DeployUtils_Cli_Deploy_APP``
will deploy the compiled test projects.

Tests named ``System_DeployUtils_Cli_Run_APP``
will run the deployed projects without any special environment.

All those tests runs at test time (ctest).


system/CMake

Checks the CMake interface.
