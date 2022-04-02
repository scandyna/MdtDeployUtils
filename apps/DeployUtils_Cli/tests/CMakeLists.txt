include(MdtAddTest)
include(MdtRuntimeEnvironment)

mdt_add_test(
  NAME CommandLineParserTest
  TARGET commandLineParserTest
  DEPENDENCIES Mdt::DeployUtils_Cli Qt5::Core Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CommandLineParserTest.cpp
)

mdt_add_test(
  NAME CommandLineParserErrorTest
  TARGET commandLineParserErrorTest
  DEPENDENCIES Mdt::DeployUtils_Cli Qt5::Core Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CommandLineParserErrorTest.cpp
)


######################################################
# mdtdeployutils --deploy-application end to end test
######################################################

set(compilerLocationOptionArguments)
if(MSVC)
  set(compilerLocationOptionArguments --compiler-location compiler-path="${CMAKE_CXX_COMPILER}")
endif()

set(deployedExecutableDirectory "${CMAKE_CURRENT_BINARY_DIR}/deployed")

add_test(NAME Apps_DeployUtils_Cli_DeployApplication
  COMMAND mdtdeployutils --log-level DEBUG deploy-application
    --shlib-overwrite-behavior overwrite
    --search-prefix-path-list "${CMAKE_PREFIX_PATH}"
    --path-list-separator ";"
    ${compilerLocationOptionArguments}
    "$<TARGET_FILE:mdtdeployutils>"
    "${deployedExecutableDirectory}"
)
mdt_set_test_library_env_path(NAME Apps_DeployUtils_Cli_DeployApplication TARGET mdtdeployutils)

# The deployed executable should run without any custom environment
add_test(NAME Apps_DeployUtils_Cli_Run_DeployedApplication
  COMMAND "${deployedExecutableDirectory}/bin/mdtdeployutils" --help
)

set_tests_properties(Apps_DeployUtils_Cli_Run_DeployedApplication
  PROPERTIES
    DEPENDS "Apps_DeployUtils_Cli_DeployApplication"
)
