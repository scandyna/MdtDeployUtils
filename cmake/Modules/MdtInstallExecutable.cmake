# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtInstallExecutable
# --------------------
#
# .. contents:: Summary
#    :local:
#
#
# Function to install a executable
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_install_executable
#
# Install a executable::
#
#   mdt_install_executable(
#     TARGET <target>
#     RUNTIME_DESTINATION <dir>
#     LIBRARY_DESTINATION <dir>
#     [EXPORT_NAME <export-name>]
#     [EXPORT_NAMESPACE <export-namespace>]
#     [NO_PACKAGE_CONFIG_FILE]
#     [EXPORT_DIRECTORY <dir>]
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#     [RUNTIME_COMPONENT <component-name>]
#     [DEVELOPMENT_COMPONENT <component-name>]
#   )
#
# Will install the executable `target` to ``CMAKE_INSTALL_PREFIX``
# into a subdirectory defined by ``RUNTIME_DESTINATION``.
#
# On Linux,
# the rpath informations is set to ``$ORIGIN/../${LIBRARY_DESTINATION}``.
# If ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ``TRUE``,
# the rpath informations are removed.
#
# To generate CMake exports, use ``EXPORT_NAME`` and ``EXPORT_NAMESPACE``.
# If specified, a file, named ``${EXPORT_NAMESPACE}${EXPORT_NAME}.cmake``, will be generated.
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
#
# See also :command:`mdt_deploy_application()`
#
# Simple example
# ^^^^^^^^^^^^^^
#
# Example:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   add_executable(myApp myApp.cpp)
#
#   mdt_install_executable(
#     TARGET myApp
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
#   )
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-bin
#        |-myApp
#
#
# Example with a package config file
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# This example illustrates a exported target:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   add_executable(mdtdeployutils DeployUtils.cpp)
#
#   mdt_install_executable(
#     TARGET mdtdeployutils
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     EXPORT_NAME DeployUtilsExecutable
#     EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
#     DEVELOPMENT_COMPONENT ${PROJECT_NAME}_Devel
#   )
#
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-bin
#     |  |-mdtdeployutils
#     |-lib
#        |-cmake
#            |-Mdt0DeployUtilsExecutable
#                 |-Mdt0DeployUtilsExecutable.cmake
#                 |-Mdt0DeployUtilsExecutableConfig.cmake
#
#
# Once the executable is installed,
# the user should be able to find it using CMake find_package() in its CMakeLists.txt:
#
# .. code-block:: cmake
#
#   find_package(Mdt0DeployUtilsExecutable REQUIRED)
#
#   add_executable(myApp myApp.cpp)
#
#   add_custom_command(TARGET myApp COMMAND Mdt0::DeployUtilsExecutable ... VERBATIM)
#
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
#   add_executable(mdtdeployutils DeployUtils.cpp)
#
#   mdt_install_executable(
#     TARGET mdtdeployutils
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     EXPORT_NAME DeployUtilsExecutable
#     EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}
#     NO_PACKAGE_CONFIG_FILE
#     EXPORT_DIRECTORY DeployUtils
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
#     DEVELOPMENT_COMPONENT ${PROJECT_NAME}_Devel
#   )
#
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-bin
#     |  |-mdtdeployutils
#     |-lib
#        |-cmake
#            |-Mdt0DeployUtils
#                 |-Mdt0DeployUtilsExecutable.cmake
#                 |-Mdt0DeployUtilsConfig.cmake
#
#
# Here ``Mdt0DeployUtilsConfig.cmake`` is not generated, but created manually.
# It includes ``Mdt0DeployUtilsExecutable.cmake``.
#
# Once the project is installed,
# the user should be able to find it using CMake find_package() in its CMakeLists.txt:
#
# .. code-block:: cmake
#
#   find_package(Mdt0DeployUtils REQUIRED)
#
#   add_executable(myApp myApp.cpp)
#
#   add_custom_command(TARGET myApp COMMAND Mdt0::DeployUtilsExecutable ... VERBATIM)
#
#
# Example of a system wide install on a Debian MultiArch (``CMAKE_INSTALL_PREFIX=/usr``)::
#
#   /usr
#     |-bin
#     |  |-mdtdeployutils
#     |-lib
#        |-x86_64-linux-gnu
#           |-cmake
#               |-Mdt0DeployUtils
#                    |-Mdt0DeployUtilsExecutable.cmake
#                    |-Mdt0DeployUtilsConfig.cmake
#

# TODO : if subdir not 1 level, document to set RPATH manually

include(MdtTargetProperties)

function(mdt_install_executable)

  set(options NO_PACKAGE_CONFIG_FILE)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION EXPORT_NAME EXPORT_NAMESPACE EXPORT_DIRECTORY INSTALL_IS_UNIX_SYSTEM_WIDE RUNTIME_COMPONENT DEVELOPMENT_COMPONENT)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_install_executable(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_install_executable(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_RUNTIME_DESTINATION)
    message(FATAL_ERROR "mdt_install_executable(): mandatory argument RUNTIME_DESTINATION missing")
  endif()
  if(NOT ARG_LIBRARY_DESTINATION)
    message(FATAL_ERROR "mdt_install_executable(): mandatory argument LIBRARY_DESTINATION missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_install_executable(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  set(runtimeComponentArguments)
  if(ARG_RUNTIME_COMPONENT)
    set(runtimeComponentArguments COMPONENT ${ARG_RUNTIME_COMPONENT})
  endif()

  set(developmentComponentArguments)
  if(ARG_DEVELOPMENT_COMPONENT)
    set(developmentComponentArguments COMPONENT ${ARG_DEVELOPMENT_COMPONENT})
  endif()

  mdt_set_target_install_rpath_property(
    TARGET ${ARG_TARGET}
    PATHS ../lib
  )

  if(ARG_EXPORT_NAME AND ARG_EXPORT_NAMESPACE)

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

    install(
      TARGETS ${ARG_TARGET}
      EXPORT ${targetExportName}
      RUNTIME
        DESTINATION "${ARG_RUNTIME_DESTINATION}"
        ${runtimeComponentArguments}
    )

    install(
      EXPORT ${targetExportName}
      DESTINATION "${cmakePackageFilesDir}"
      NAMESPACE ${ARG_EXPORT_NAMESPACE}::
      FILE ${targetExportName}.cmake
      ${developmentComponentArguments}
    )

    if(NOT ARG_NO_PACKAGE_CONFIG_FILE)

      set(cmakePackageConfigFileContent "include(\"\${CMAKE_CURRENT_LIST_DIR}/${targetExportName}.cmake\")\n")

      set(cmakePackageConfigFile "${CMAKE_CURRENT_BINARY_DIR}/${targetExportName}Config.cmake")

      file(WRITE "${cmakePackageConfigFile}" "${cmakePackageConfigFileContent}")

      install(
        FILES ${cmakePackageConfigFile}
        DESTINATION "${cmakePackageFilesDir}"
        ${developmentComponentArguments}
      )

    endif()

  else()

    install(
      TARGETS ${ARG_TARGET}
      RUNTIME
        DESTINATION "${ARG_RUNTIME_DESTINATION}"
        ${runtimeComponentArguments}
    )

  endif()

endfunction()
