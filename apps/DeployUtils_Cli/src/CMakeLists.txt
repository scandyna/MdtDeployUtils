
add_library(Mdt_DeployUtils_Cli STATIC
  DeployUtilsMain.cpp
  CommandLineParser.cpp
  GetSharedLibrariesTargetDependsOnCommandLineParser.cpp
  CopySharedLibrariesTargetDependsOnCommandLineParser.cpp
  CommandLineCommand.cpp
  MessageLogger.cpp
)
add_library(Mdt::DeployUtils_Cli ALIAS Mdt_DeployUtils_Cli)

if(BUILD_USE_IPO_LTO)
  set_target_properties(Mdt_DeployUtils_Cli
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_include_directories(Mdt_DeployUtils_Cli
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(Mdt_DeployUtils_Cli
  PUBLIC
    Mdt::DeployUtils
    Mdt0::ConsoleApplication
    Qt5::Core
)


add_executable(mdtdeployutils
  main.cpp
)

if(BUILD_USE_IPO_LTO)
  set_target_properties(mdtdeployutils
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_include_directories(mdtdeployutils
  PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(mdtdeployutils
  PRIVATE
    Mdt::DeployUtils_Cli
)
