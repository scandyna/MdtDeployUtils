# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(MdtRuntimeEnvironment)

set(compilerLocationOptionArguments)
if(MSVC)
  set(compilerLocationOptionArguments --compiler-location compiler-path="${CMAKE_CXX_COMPILER}")
endif()

set(deployedExecutableDirectory "${CMAKE_CURRENT_BINARY_DIR}/deployed")

add_test(NAME System_DeployUtils_Cli_Deploy_QtCoreApp
  COMMAND mdtdeployutils --log-level DEBUG deploy-application
    --shlib-overwrite-behavior overwrite
    --search-prefix-path-list "${CMAKE_PREFIX_PATH}"
    ${compilerLocationOptionArguments}
    "$<TARGET_FILE:qtcoreapp>"
    "${deployedExecutableDirectory}"
)
mdt_set_test_library_env_path(NAME System_DeployUtils_Cli_Deploy_QtCoreApp TARGET mdtdeployutils)

# The deployed executable should run without any custom environment
add_test(NAME System_DeployUtils_Cli_Run_QtCoreApp
  COMMAND "${deployedExecutableDirectory}/bin/qtcoreapp"
)

set_tests_properties(System_DeployUtils_Cli_Run_QtCoreApp
  PROPERTIES
    DEPENDS "System_DeployUtils_Cli_Deploy_QtCoreApp"
)
