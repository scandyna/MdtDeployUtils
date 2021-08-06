# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

include(MdtDeployApplication)
include(MdtInstallExecutable) # TODO: remove

add_library(Mdt_DeployUtils_Cli STATIC
  DeployUtilsMain.cpp
  CopySharedLibrariesTargetDependsOnCommandLineParserDefinition.cpp
  CommandLineParser.cpp
  CommandLineCommand.cpp
)
add_library(Mdt::DeployUtils_Cli ALIAS Mdt_DeployUtils_Cli)

target_compile_features(Mdt_DeployUtils_Cli PUBLIC cxx_std_14)

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
    Mdt::DeployUtilsCore
    Mdt::CommandLineParser
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

# TODO: re-enable
# mdt_deploy_application(
#   TARGET mdtdeployutils
#   RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#   LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#   COMPONENT ${PROJECT_NAME}_Runtime
# )

# TODO: remove
mdt_install_executable(
  TARGET mdtdeployutils
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  EXPORT_NAME DeployUtilsExecutable
  EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}
  NO_PACKAGE_CONFIG_FILE
  EXPORT_DIRECTORY DeployUtils
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
  RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
  DEVELOPMENT_COMPONENT ${PROJECT_NAME}_Dev
)
