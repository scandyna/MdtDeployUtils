# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# MdtDeployApplication
# --------------------
#
# .. contents:: Summary
#    :local:
#
# Deploy a application
# ^^^^^^^^^^^^^^^^^^^^
#
# .. command:: mdt_deploy_application
#
# Deploy a application with `target` as executable::
#
#   mdt_deploy_application(
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
#     [EXCLUDE_FROM_ALL]
#   )
#
# Will install `target` using :command:`mdt_install_executable()`.
#
# For the usage of arguments not documented here,
# see :command:`mdt_install_executable()` from the
# `Mdt CMake Modules MdtInstallExecutable module <https://scandyna.gitlab.io/mdt-cmake-modules/Modules/MdtInstallExecutable.html>`_ .
#
# The shared libraries the executable depends on
# are also installed using :command:`mdt_install_shared_libraries_target_depends_on()`.
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
#   target_link_libraries(myApp PRIVATE Qt5::Core)
#   target_link_libraries(myApp PRIVATE Mdt0::PlainText)
#   target_link_libraries(myApp PRIVATE myLibA)
#
#   mdt_deploy_application(
#     TARGET myApp
#     RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#   )
#

include(MdtInstallExecutable)
include(MdtSharedLibrariesDepencyHelpers)

function(mdt_deploy_application)

  set(options NO_PACKAGE_CONFIG_FILE EXCLUDE_FROM_ALL)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION EXPORT_NAME EXPORT_NAMESPACE EXPORT_DIRECTORY INSTALL_IS_UNIX_SYSTEM_WIDE RUNTIME_COMPONENT DEVELOPMENT_COMPONENT)
  set(multiValueArgs)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_TARGET)
    message(FATAL_ERROR "mdt_deploy_application(): no target provided")
  endif()
  if(NOT TARGET ${ARG_TARGET})
    message(FATAL_ERROR "mdt_deploy_application(): ${ARG_TARGET} is not a valid target")
  endif()
  if(NOT ARG_RUNTIME_DESTINATION)
    message(FATAL_ERROR "mdt_deploy_application(): mandatory argument RUNTIME_DESTINATION missing")
  endif()
  if(NOT ARG_LIBRARY_DESTINATION)
    message(FATAL_ERROR "mdt_deploy_application(): mandatory argument LIBRARY_DESTINATION missing")
  endif()
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_deploy_application(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  set(exportNameArguments)
  if(ARG_EXPORT_NAME)
    set(exportNameArguments EXPORT_NAME ${ARG_EXPORT_NAME})
  endif()

  set(exportNamespaceArguments)
  if(ARG_EXPORT_NAMESPACE)
    set(exportNamespaceArguments EXPORT_NAMESPACE ${ARG_EXPORT_NAMESPACE})
  endif()

  set(noPackageConfigFileArguments)
  if(ARG_NO_PACKAGE_CONFIG_FILE)
    set(noPackageConfigFileArguments NO_PACKAGE_CONFIG_FILE)
  endif()

  set(exportDirectoryArguments)
  if(ARG_EXPORT_DIRECTORY)
    set(exportDirectoryArguments EXPORT_DIRECTORY ${ARG_EXPORT_DIRECTORY})
  endif()

  set(installIsUnixSystemWideArguments)
  if(ARG_INSTALL_IS_UNIX_SYSTEM_WIDE)
    set(installIsUnixSystemWideArguments INSTALL_IS_UNIX_SYSTEM_WIDE ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE})
  endif()

  set(runtimeComponentArguments)
  if(ARG_RUNTIME_COMPONENT)
    set(runtimeComponentArguments RUNTIME_COMPONENT ${ARG_RUNTIME_COMPONENT})
  endif()

  set(developmentComponentArguments)
  if(ARG_DEVELOPMENT_COMPONENT)
    set(developmentComponentArguments DEVELOPMENT_COMPONENT ${ARG_DEVELOPMENT_COMPONENT})
  endif()

  set(excludeFromAllArgument)
  if(ARG_EXCLUDE_FROM_ALL)
    set(excludeFromAllArgument EXCLUDE_FROM_ALL)
  endif()

  mdt_install_executable(
    TARGET ${ARG_TARGET}
    RUNTIME_DESTINATION ${ARG_RUNTIME_DESTINATION}
    LIBRARY_DESTINATION ${ARG_LIBRARY_DESTINATION}
    ${exportNameArguments}
    ${exportNamespaceArguments}
    ${noPackageConfigFileArguments}
    ${exportDirectoryArguments}
    ${installIsUnixSystemWideArguments}
    ${runtimeComponentArguments}
    ${developmentComponentArguments}
    ${excludeFromAllArgument}
  )

  set(componentArguments)
  if(ARG_RUNTIME_COMPONENT)
    set(componentArguments COMPONENT ${ARG_RUNTIME_COMPONENT})
  endif()

  mdt_install_shared_libraries_target_depends_on(
    TARGET ${ARG_TARGET}
    RUNTIME_DESTINATION ${ARG_RUNTIME_DESTINATION}
    LIBRARY_DESTINATION ${ARG_LIBRARY_DESTINATION}
    INSTALL_IS_UNIX_SYSTEM_WIDE ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE}
    ${componentArguments}
    ${excludeFromAllArgument}
  )

endfunction()
