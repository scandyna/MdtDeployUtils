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
#     [LIBRARY_DESTINATION <dir>]
#     [EXPORT_DIRECTORY <dir>]
#     [EXPORT_NAME <export-name>]
#     [EXPORT_NAMESPACE <export-namespace>]
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#     [COMPONENT <component-name>]
#     [RUNTIME_COMPONENT <component-name>] TODO remove
#     [DEVELOPMENT_COMPONENT <component-name>] TODO remove
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
# TODO: finish doc
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
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     COMPONENT ${PROJECT_NAME}_Runtime
#   )
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-bin
#     |  |-myApp
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
#     COMPONENT ${PROJECT_NAME}_Runtime
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
#   add_custom_command(TARGET myApp COMMAND mdtdeployutils ... VERBATIM)
#   OR
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
#     EXPORT_DIRECTORY DeployUtils
#     EXPORT_NAME DeployUtilsExecutable
#     EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     COMPONENT ${PROJECT_NAME}_Runtime
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
# Here ``Mdt0DeployUtilsConfig.cmake`` includes ``Mdt0DeployUtilsExecutable.cmake``.
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
#   add_custom_command(TARGET myApp COMMAND mdtdeployutils ... VERBATIM)
#   OR
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

function(mdt_install_executable)

  set(options)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION INSTALL_IS_UNIX_SYSTEM_WIDE COMPONENT)
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

  set(componentArguments)
  if(ARG_COMPONENT)
    set(componentArguments COMPONENT ${ARG_COMPONENT})
  endif()

  install(
    TARGETS ${ARG_TARGET}
    RUNTIME DESTINATION "${ARG_RUNTIME_DESTINATION}"
    ${componentArguments}
  )
  
  # TODO: finish implementation

endfunction()
