# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(MdtRuntimeEnvironment)

set(compilerLocationOptionArguments)
if(MSVC)
  set(compilerLocationOptionArguments --compiler-location compiler-path="${CMAKE_CXX_COMPILER}")
endif()

######################################################
# Deploy and run a QtCore based application
######################################################

set(qtCoreAppDeployedExecutableDirectory "${CMAKE_CURRENT_BINARY_DIR}/deployed/QtCoreApp")

add_test(NAME System_DeployUtils_Cli_Deploy_QtCoreApp
  COMMAND mdtdeployutils --log-level DEBUG deploy-application
    --shlib-overwrite-behavior overwrite
    --search-prefix-path-list "${CMAKE_PREFIX_PATH}"
    --path-list-separator ";"
    ${compilerLocationOptionArguments}
    "$<TARGET_FILE:qtcoreapp>"
    "${qtCoreAppDeployedExecutableDirectory}"
)
mdt_set_test_library_env_path(NAME System_DeployUtils_Cli_Deploy_QtCoreApp TARGET mdtdeployutils)

# The deployed executable should run without any custom environment
add_test(NAME System_DeployUtils_Cli_Run_QtCoreApp
  COMMAND "${qtCoreAppDeployedExecutableDirectory}/bin/qtcoreapp"
)

set_tests_properties(System_DeployUtils_Cli_Run_QtCoreApp
  PROPERTIES
    DEPENDS "System_DeployUtils_Cli_Deploy_QtCoreApp"
)

######################################################
# Deploy and run a QtWidgets based application
######################################################

set(qtWidgetsAppDeployedExecutableDirectory "${CMAKE_CURRENT_BINARY_DIR}/deployed/QtWidgetsApp")

add_test(NAME System_DeployUtils_Cli_Deploy_QtWidgetsApp
  COMMAND mdtdeployutils --log-level DEBUG deploy-application
    --shlib-overwrite-behavior overwrite
    --search-prefix-path-list "${CMAKE_PREFIX_PATH}"
    --path-list-separator ";"
    ${compilerLocationOptionArguments}
    "$<TARGET_FILE:qtwidgetsapp>"
    "${qtWidgetsAppDeployedExecutableDirectory}"
)
mdt_set_test_library_env_path(NAME System_DeployUtils_Cli_Deploy_QtWidgetsApp TARGET mdtdeployutils)

# The deployed executable should run without any custom environment
add_test(NAME System_DeployUtils_Cli_Run_QtWidgetsApp
  COMMAND "${qtWidgetsAppDeployedExecutableDirectory}/bin/qtwidgetsapp"
)

set_tests_properties(System_DeployUtils_Cli_Run_QtWidgetsApp
  PROPERTIES
    DEPENDS "System_DeployUtils_Cli_Deploy_QtWidgetsApp"
    ENVIRONMENT QT_DEBUG_PLUGINS=1
)
