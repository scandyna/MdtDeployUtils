
# add_library(Mdt_DeployUtils_Cli_BashCompletion STATIC
#   BashCompletionGeneratorOption.cpp
#   BashCompletionGeneratorArgument.cpp
#   BashCompletionGeneratorCommand.cpp
#   BashCompletionGenerator.cpp
# )
# add_library(Mdt::DeployUtils_Cli_BashCompletion ALIAS Mdt_DeployUtils_Cli_BashCompletion)
# 
# if(BUILD_USE_IPO_LTO)
#   set_target_properties(Mdt_DeployUtils_Cli_BashCompletion
#     PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
#   )
# endif()
# 
# target_include_directories(Mdt_DeployUtils_Cli_BashCompletion
#   PUBLIC
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
# )
# 
# # Depends on DeployUtils for Algorithm
# target_link_libraries(Mdt_DeployUtils_Cli_BashCompletion
#   PUBLIC
#     Mdt::DeployUtils
#     Qt5::Core
# )


add_executable(mdtdeployutilsBashCompletionScriptGenerator
  main.cpp
  DeployUtilsBashCompletionGenerator.cpp
)

if(BUILD_USE_IPO_LTO)
  set_target_properties(mdtdeployutilsBashCompletionScriptGenerator
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_include_directories(mdtdeployutilsBashCompletionScriptGenerator
  PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(mdtdeployutilsBashCompletionScriptGenerator
  PRIVATE
    Mdt::CommandLineParser
    Mdt::DeployUtils_Cli
)

add_custom_target(generateMdtdeployutilsBashCompletionScript ALL
  COMMAND mdtdeployutilsBashCompletionScriptGenerator "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "Generating mdtdeployutils-completion.bash"
  VERBATIM
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/mdtdeployutils"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/bash-completion/completions"
  COMPONENT ${PROJECT_NAME}_Runtime
)

# add_custom_command(
#   OUTPUT "mdtdeployutils-completion.bash"
#   COMMAND mdtdeployutilsBashCompletionScriptGenerator "${CMAKE_CURRENT_BINARY_DIR}"
#   DEPENDS mdtdeployutilsBashCompletionScriptGenerator
#   COMMENT "Generating mdtdeployutils-completion.bash"
#   VERBATIM
# )

# add_custom_command(
#   OUTPUT mdtdeployutils-completion.bash
#   COMMAND generateMdtdeployutilsBashCompletionScript "$<TARGET_FILE:mdtdeployutils-completion.bash>"
#   COMMENT "Generating mdtdeployutils-completion.bash"
#   VERBATIM
# )
