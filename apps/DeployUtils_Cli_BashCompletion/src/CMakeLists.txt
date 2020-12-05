
add_library(Mdt_DeployUtils_Cli_BashCompletion STATIC
  BashCompletionGeneratorOption.cpp
  BashCompletionGeneratorArgument.cpp
  BashCompletionGeneratorCommand.cpp
  BashCompletionGenerator.cpp
)
add_library(Mdt::DeployUtils_Cli_BashCompletion ALIAS Mdt_DeployUtils_Cli_BashCompletion)

if(BUILD_USE_IPO_LTO)
  set_target_properties(Mdt_DeployUtils_Cli_BashCompletion
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_include_directories(Mdt_DeployUtils_Cli_BashCompletion
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# Depends on DeployUtils for Algorithm
target_link_libraries(Mdt_DeployUtils_Cli_BashCompletion
  PUBLIC
    Mdt::DeployUtils
)


add_executable(generateMdtdeployutilsBashCompletionScript
  main.cpp
  DeployUtilsBashCompletionGenerator.cpp
)

if(BUILD_USE_IPO_LTO)
  set_target_properties(generateMdtdeployutilsBashCompletionScript
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_include_directories(generateMdtdeployutilsBashCompletionScript
  PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(generateMdtdeployutilsBashCompletionScript
  PRIVATE
    Mdt::DeployUtils_Cli_BashCompletion
    Mdt::DeployUtils
)
