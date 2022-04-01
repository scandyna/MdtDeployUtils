# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtDeployUtilsPackageConfigHelpers
# ----------------------------------
#
# .. contents:: Summary
#    :local:
#
#
# Function to install package config files for a executable
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_deployutils_install_executable_package_config_files
#
# Note that this is a low level function
# that is used by other high level functions.
#
# See also :command:`mdt_deploy_application()`
# and :command:`mdt_install_shared_libraries_target_depends_on()`.
#
# Install a executable::
#
#   mdt_deployutils_install_executable_package_config_files(
#     TARGET <target>
#     RUNTIME_DESTINATION <dir>
#     LIBRARY_DESTINATION <dir>
#     EXPORT_NAME <export-name>
#     EXPORT_NAMESPACE <export-namespace>
#     [NO_PACKAGE_CONFIG_FILE]
#     [EXPORT_DIRECTORY <dir>]
#     [RUNTIME_COMPONENT <component-name>]
#     [DEVELOPMENT_COMPONENT <component-name>]
#     [EXCLUDE_FROM_ALL]
#   )
#
# Will generate a file, named ``${EXPORT_NAMESPACE}${EXPORT_NAME}.cmake``.
# This file will define a ``IMPORTED`` target, named ``${EXPORT_NAMESPACE}::${EXPORT_NAME}``.
# If ``NO_PACKAGE_CONFIG_FILE`` is not set,
# a CMake package config file, named ``${EXPORT_NAMESPACE}${EXPORT_NAME}Config.cmake``, will be generated.
#
# By default, those generated files are installed to ``${LIBRARY_DESTINATION}/cmake/${EXPORT_NAMESPACE}${EXPORT_NAME}``.
# If ``EXPORT_DIRECTORY`` is set,
# those generated files are installed to ``${LIBRARY_DESTINATION}/cmake/${EXPORT_NAMESPACE}${EXPORT_DIRECTORY}``.
#
# If ``RUNTIME_COMPONENT`` is specified, the target installation will be part of it.
#
# If ``DEVELOPMENT_COMPONENT`` is specified, the CMake exports and package config files installation will be part of it.
#
# During a full installation all components are installed unless marked with ``EXCLUDE_FROM_ALL`` .
#
# Example
# ^^^^^^^
#
# This example illustrates a exported target:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   add_executable(mytool MyTool.cpp)
#
#   mdt_deployutils_install_executable_package_config_files(
#     TARGET mytool
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     EXPORT_NAME MyToolExecutable
#     EXPORT_NAMESPACE MyLib${PROJECT_VERSION_MAJOR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
#     DEVELOPMENT_COMPONENT ${PROJECT_NAME}_Devel
#   )
#
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-lib
#        |-cmake
#            |-MyLib0MyToolExecutable
#                 |-MyLib0MyToolExecutable.cmake
#                 |-MyLib0MyToolExecutableConfig.cmake
#
#
# Once the executable is installed,
# the user should be able to find it using CMake find_package() in its CMakeLists.txt:
#
# .. code-block:: cmake
#
#   find_package(MyLib0MyToolExecutable REQUIRED)
#
#   add_executable(userapp UserApp.cpp)
#
#   add_custom_command(TARGET userapp COMMAND MyLib0::MyToolExecutable ... VERBATIM)
#
# Example with a package config file part of a project
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# This example illustrates how to install a tool
# that is part of a project, and that can later be imported
# from a other project:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   add_executable(mytool MyTool.cpp)
#
#   mdt_deployutils_install_executable_package_config_files(
#     TARGET mytool
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     EXPORT_NAME MyToolExecutable
#     EXPORT_NAMESPACE MyLib${PROJECT_VERSION_MAJOR}
#     NO_PACKAGE_CONFIG_FILE
#     EXPORT_DIRECTORY MyTools
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
#     DEVELOPMENT_COMPONENT ${PROJECT_NAME}_Devel
#   )
#
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-lib
#        |-cmake
#            |-MyLib0MyTools
#                 |-MyLib0MyToolExecutable.cmake
#                 |-MyLib0MyToolsConfig.cmake
#
# Here ``MyLib0MyToolsConfig.cmake`` is not generated, but created manually.
# It includes ``MyLib0MyToolExecutable.cmake``.
#
# Once the project is installed,
# the user should be able to find it using CMake find_package() in its CMakeLists.txt:
#
# .. code-block:: cmake
#
#   find_package(MyLib0MyTools REQUIRED)
#
#   add_executable(userapp UserApp.cpp)
#
#   add_custom_command(TARGET userapp COMMAND MyLib0::MyToolExecutable ... VERBATIM)
#
function(mdt_deployutils_install_executable_package_config_files)

  set(options NO_PACKAGE_CONFIG_FILE EXCLUDE_FROM_ALL)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION EXPORT_NAME EXPORT_NAMESPACE EXPORT_DIRECTORY RUNTIME_COMPONENT DEVELOPMENT_COMPONENT)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_deployutils_install_executable_package_config_files(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_deployutils_install_executable_package_config_files(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_RUNTIME_DESTINATION)
    message(FATAL_ERROR "mdt_deployutils_install_executable_package_config_files(): mandatory argument RUNTIME_DESTINATION missing")
  endif()
  if(NOT ARG_LIBRARY_DESTINATION)
    message(FATAL_ERROR "mdt_deployutils_install_executable_package_config_files(): mandatory argument LIBRARY_DESTINATION missing")
  endif()
  if(NOT ARG_EXPORT_NAME)
    message(FATAL_ERROR "mdt_deployutils_install_executable_package_config_files(): mandatory argument EXPORT_NAME missing")
  endif()
  if(NOT ARG_EXPORT_NAMESPACE)
    message(FATAL_ERROR "mdt_deployutils_install_executable_package_config_files(): mandatory argument EXPORT_NAMESPACE missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_deployutils_install_executable_package_config_files(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(ARG_EXPORT_DIRECTORY)
    set(cmakePackageFilesDir "${ARG_LIBRARY_DESTINATION}/cmake/${ARG_EXPORT_NAMESPACE}${ARG_EXPORT_DIRECTORY}")
  else()
    set(cmakePackageFilesDir "${ARG_LIBRARY_DESTINATION}/cmake/${ARG_EXPORT_NAMESPACE}${ARG_EXPORT_NAME}")
  endif()

  set(targetExportName ${ARG_EXPORT_NAMESPACE}${ARG_EXPORT_NAME})

  set_target_properties(${ARG_TARGET}
    PROPERTIES
      EXPORT_NAME ${ARG_EXPORT_NAME}
  )

  set(runtimeComponentArguments)
  if(ARG_RUNTIME_COMPONENT)
    set(runtimeComponentArguments COMPONENT ${ARG_RUNTIME_COMPONENT})
  endif()

  set(developmentComponentArguments)
  if(ARG_DEVELOPMENT_COMPONENT)
    set(developmentComponentArguments COMPONENT ${ARG_DEVELOPMENT_COMPONENT})
  endif()

  set(excludeFromAllArgument)
  if(ARG_EXCLUDE_FROM_ALL)
    set(excludeFromAllArgument EXCLUDE_FROM_ALL)
  endif()

  message(DEBUG "install EXPORTS for ${ARG_TARGET}")
  message(DEBUG "Runtime COMPONENT args: ${runtimeComponentArguments}")
  message(DEBUG "Dev COMPONENT args: ${developmentComponentArguments}")

  # TODO: see if it is possible to generate a correct export without really install the target
  #       here we install the target, then it will be overwritten by mdtdeployutils
  #       also, hope that mdtdeployutils will be called after this install !
  install(
    TARGETS ${ARG_TARGET}
    EXPORT ${targetExportName}
    RUNTIME
      DESTINATION "${ARG_RUNTIME_DESTINATION}"
      ${runtimeComponentArguments}
      ${excludeFromAllArgument}
  )
  # export() will probably not work
  # see CMake documentation for why
#     export(
#       TARGETS ${ARG_TARGET}
#       FILE ${targetExportName}.cmake
#     )

  install(
    EXPORT ${targetExportName}
    DESTINATION "${cmakePackageFilesDir}"
    NAMESPACE ${ARG_EXPORT_NAMESPACE}::
    FILE ${targetExportName}.cmake
    ${developmentComponentArguments}
    ${excludeFromAllArgument}
  )

  if(NOT ARG_NO_PACKAGE_CONFIG_FILE)

    set(cmakePackageConfigFileContent "include(\"\${CMAKE_CURRENT_LIST_DIR}/${targetExportName}.cmake\")\n")

    set(cmakePackageConfigFile "${CMAKE_CURRENT_BINARY_DIR}/${targetExportName}Config.cmake")

    file(WRITE "${cmakePackageConfigFile}" "${cmakePackageConfigFileContent}")

    install(
      FILES ${cmakePackageConfigFile}
      DESTINATION "${cmakePackageFilesDir}"
      ${developmentComponentArguments}
      ${excludeFromAllArgument}
    )

  endif()

endfunction()
