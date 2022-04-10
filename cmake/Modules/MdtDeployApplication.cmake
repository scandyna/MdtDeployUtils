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
#     [QT_PLUGINS_SET <set>]
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
# Will install the executable `target` to ``CMAKE_INSTALL_PREFIX``
# into a subdirectory defined by ``RUNTIME_DESTINATION``.
#
# On systems that supports rpath, if ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is not set, or set to ``FALSE``,
# the rpath informations is set to a relative path so that the executable finds its shared libraries
# (for example, on a standard Linux layout, ``$ORIGIN/../${LIBRARY_DESTINATION}``).
#
# The shared libraries ``target`` depends on are installed to ``CMAKE_INSTALL_PREFIX``,
# into a appropriate subdirectory.
# On UNIX, the subdirectory is the one specified by ``LIBRARY_DESTINATION``.
# On Windows, the subdirectory is the one specified by ``RUNTIME_DESTINATION``.
#
# For some more details about those shared libraries,
# see :command:`mdt_install_shared_libraries_target_depends_on()`.
#
# If ``target`` depends on any Qt shared library, directly or indirectly,
# the required Qt plugins will also be installed in a appropriate subdirectory.
# The additional shared libraries those Qt plugins depends on
# will also be installed as described above.
#
# The required Qt plugins is choosen depending on Qt shared libraries present in the dependencies.
# For example, if the shared library for the Qt GUI module is present (for example libQt5Gui.so),
# all plugins from imageformats, platforms, etc will be deployed.
# To limit the plugins to deploy, it is possible to define a set:
#
# .. code-block:: cmake
#
#   QT_PLUGINS_SET "imageformats:jpeg,svg|platforms:xcb,vnc,eglfs,windows,direct2d"
#
# In above example, only jpeg and svg imageformats plugins will be deployed.
# For platform plugins, we expressed plugins for Linux and Windows.
# On Linux, only xcb, vnc and eglfs will be deployed. On Windows, only windows and direct2d plugins will be deployed.
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
# During a full installation all components are installed unless marked with ``EXCLUDE_FROM_ALL`` .
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
#   target_link_libraries(myApp PRIVATE Qt5::Widgets)
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
#   mdt_deploy_application(
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

include(MdtDeployUtilsPackageConfigHelpers)
include(MdtGenerateMdtdeployutilsInstallScript)

function(mdt_deploy_application)

  set(options NO_PACKAGE_CONFIG_FILE EXCLUDE_FROM_ALL)
  set(oneValueArgs TARGET RUNTIME_DESTINATION LIBRARY_DESTINATION QT_PLUGINS_SET EXPORT_NAME EXPORT_NAMESPACE EXPORT_DIRECTORY INSTALL_IS_UNIX_SYSTEM_WIDE RUNTIME_COMPONENT DEVELOPMENT_COMPONENT)
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

  set(componentArguments)
  if(ARG_RUNTIME_COMPONENT)
    set(componentArguments COMPONENT ${ARG_RUNTIME_COMPONENT})
  endif()


  if(ARG_EXPORT_NAME AND ARG_EXPORT_NAMESPACE)

    set(noPackageConfigFileArguments)
    if(ARG_NO_PACKAGE_CONFIG_FILE)
      set(noPackageConfigFileArguments NO_PACKAGE_CONFIG_FILE)
    endif()

    set(exportDirectoryArguments)
    if(ARG_EXPORT_DIRECTORY)
      set(exportDirectoryArguments EXPORT_DIRECTORY ${ARG_EXPORT_DIRECTORY})
    endif()

    mdt_deployutils_install_executable_package_config_files(
      TARGET ${ARG_TARGET}
      RUNTIME_DESTINATION "${ARG_RUNTIME_DESTINATION}"
      LIBRARY_DESTINATION "${ARG_LIBRARY_DESTINATION}"
      EXPORT_NAME ${ARG_EXPORT_NAME}
      EXPORT_NAMESPACE ${ARG_EXPORT_NAMESPACE}
      ${noPackageConfigFileArguments}
      ${exportDirectoryArguments}
      ${runtimeComponentArguments}
      ${developmentComponentArguments}
      ${excludeFromAllArgument}
    )

  endif()

  set(MDT_DEPLOY_APPLICATION_INSTALL_SCRIPT_INSTALL_IS_UNIX_SYSTEM_WIDE ${ARG_INSTALL_IS_UNIX_SYSTEM_WIDE})
  set(MDT_DEPLOY_APPLICATION_INSTALL_SCRIPT_RUNTIME_DESTINATION ${ARG_RUNTIME_DESTINATION})
  set(MDT_DEPLOY_APPLICATION_INSTALL_SCRIPT_LIBRARY_DESTINATION ${ARG_LIBRARY_DESTINATION})
  set(MDT_DEPLOY_APPLICATION_INSTALL_SCRIPT_QT_PLUGINS_SET ${ARG_QT_PLUGINS_SET})

  mdt_generate_mdtdeployutils_install_script(
    TARGET ${ARG_TARGET}
    INPUT_SCRIPT_FILE MdtDeployApplicationInstallScript.cmake.in
    ${componentArguments}
    ${excludeFromAllArgument}
  )

endfunction()
