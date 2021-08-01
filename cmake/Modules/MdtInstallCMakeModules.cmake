# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# TODO: document, implement
# TODO: should go to MdtCMakeModules

#.rst:
# MdtInstallCMakeModules
# ----------------------
#
# .. contents:: Summary
#    :local:
#
#
# Functions
# ^^^^^^^^^
#
# .. command:: mdt_install_cmake_modules
#
# Install a set of CMake modules::
#
#   mdt_install_cmake_modules(
#     FILES files...
#     [DESTINATION <dir>]
#     [EXPORT_DIRECTORY <dir>]
#     EXPORT_NAME <export-name>
#     EXPORT_NAMESPACE <export-namespace>  TODO: not used
#     INSTALL_NAMESPACE <install-namespace>
#     [INSTALL_IS_UNIX_SYSTEM_WIDE [TRUE|FALSE]]
#     [TOOLS executable1 [executable2 ...]] TODO: remove
#     [LINUX_TOOLS executable1 [executable2 ...]] TODO: remove
#     [WINDOWS_TOOLS executable1 [executable2 ...]] TODO: remove
#     [APPLE_TOOLS executable1 [executable2 ...]] TODO: remove
#     [COMPONENT <component-name>]
#     [NO_PACKAGE_CONFIG_FILES]
#   )
#
# Install the CMake modules designated by ``files`` using :command:`install(FILES)`.
#
# By default, the destination is relative to ``CMAKE_INSTALL_PREFIX`` and depends on ``INSTALL_IS_UNIX_SYSTEM_WIDE``:
# if it is ``TRUE``, it will be ``${CMAKE_INSTALL_DATADIR}/<package-name>/Modules``, otherwise ``cmake/Modules``.
#
# Alternatively, it is possible to specify a directory (or a relative path) using the ``DESTINATION`` argument.
# This directory will be relative to ``CMAKE_INSTALL_PREFIX``.
# When using ``DESTINATION``, ``INSTALL_IS_UNIX_SYSTEM_WIDE`` is ignored.
#
# ``<package-name>`` is named ``${INSTALL_NAMESPACE}${EXPORT_NAME}``.
#
# If some modules requires tools, for example ``ldd``, it can be passed as ``TOOLS`` arguments.
# Each argument is a executable name, without its ``.exe`` extension.
# Internally, :command:`find_program()` will be used to find each tool.
#
# TODO: remove tools part
#
# The ``LINUX_TOOLS`` argument is the same as ``TOOLS``,
# but is only used on Linux hosts. (i.e. ``CMAKE_HOST_UNIX`` is `true` and ``CMAKE_HOST_APPLE`` is `false`).
#
# The ``WINDOWS_TOOLS`` argument is the same as ``TOOLS``,
# but is only used on Windows hosts. (i.e. ``CMAKE_HOST_WIN32`` is `true`).
#
# The ``APPLE_TOOLS`` argument is the same as ``TOOLS``,
# but is only used on Apple hosts. (i.e. ``CMAKE_HOST_APPLE`` is `true`).
#
# Notice that the tools will not be installed by this function,
# but :command:`find_program()` calls will be generated
# in the package config files to find them.
#
# A package config file, named ``<package-name>Config.cmake``,
# will be generated and installed in a location known by :command:`find_package()`.
#
# It is possible to not generate the package config files by setting
# the ``NO_PACKAGE_CONFIG_FILES`` option.
#
#
# Example:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   set(MDT_INSTALL_PACKAGE_NAME Mdt0)
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   mdt_install_cmake_modules(
#     FILES
#       Modules/ModuleA.cmake
#       Modules/ModuleB.cmake
#     EXPORT_NAME DeployUtilsCMake
#     EXPORT_NAMESPACE Mdt0::  TODO: not used
#     INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
#     INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
#     TOOLS myTool TODO: remove
#     LINUX_TOOLS ldd TODO: remove
#     WINDOWS_TOOLS objdump TODO: remove
#     APPLE_TOOLS otool TODO: remove
#                         NOTE: could be a target: DeployUtils (which contains tools)
#     COMPONENT Runtime
#   )
#
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     |-cmake
#        |-Modules
#        |  |-ModuleA.cmake
#        |  |-ModuleB.cmake
#        |-Mdt0DeployUtilsCMake
#             |-Mdt0DeployUtilsCMakeConfig.cmake
#
#
# Example of a system wide install on a Debian MultiArch (``CMAKE_INSTALL_PREFIX=/usr``)::
#
#   /usr
#     |-share
#         |-Mdt0DeployUtilsCMake
#         |   |-Modules
#         |      |-ModuleA.cmake
#         |      |-ModuleB.cmake
#         |-cmake
#             |-Mdt0DeployUtilsCMake
#                 |-Mdt0DeployUtilsCMakeConfig.cmake
#
#
#
# Example to install CMake modules that are part of a project having other parts:
#
# .. code-block:: cmake
#
#   # This should be set at the top level CMakeLists.txt
#   set(MDT_INSTALL_PACKAGE_NAME Mdt0)
#   include(GNUInstallDirs)
#   include(MdtInstallDirs)
#
#   mdt_install_cmake_modules(
#     FILES
#       Modules/ModuleA.cmake
#       Modules/ModuleB.cmake
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     EXPORT_DIRECTORY DeployUtils
#     EXPORT_NAME DeployUtilsModules
#     INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
#     COMPONENT ${PROJECT_NAME}_Runtime
#   )
#
# On a non system wide Linux installation, the result will be::
#
#   ${CMAKE_INSTALL_PREFIX}
#     lib
#      |-cmake
#         |-Modules
#         |  |-ModuleA.cmake
#         |  |-ModuleB.cmake
#         |-Mdt0DeployUtils
#              |-Mdt0DeployUtilsModulesConfig.cmake
#
#
# Example of a system wide install on a Debian MultiArch (``CMAKE_INSTALL_PREFIX=/usr``)::
#
#     /usr
#       |-lib
#          |-x86_64-linux-gnu
#             |-cmake
#                |-Modules
#                |  |-ModuleA.cmake
#                |  |-ModuleB.cmake
#                |-Mdt0DeployUtils
#                     |-Mdt0DeployUtilsModulesConfig.cmake
#
#
# TODO: document project config file that includes the module config file
#
# Install modules that requires tools
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# Some modules can require tools, like ``ldd``, ``objdump`` or a custom tool.
#
# One solution could be to find those tools when the user calls ``find_package()``::
#
#   find_package(MyCMakeModules REQUIRED)
#   # Here, 2 targets are defined: ldd and myTool
#   # Those are imported targets
#
# This seems nice, but it can create name clashes.
# What is the consumer project also needs ``ldd``,
# and has previously defined it with ``find_program()`` ?
# Try to define aliases could be a solution to the name clashes.
# Another problem is that some tools are searched multiple times,
# which can be slower, but also could result to unexpected versions.
#
# A better solution is to be explicit::
#
#   find_package(MyCMakeModules REQUIRED)
#   find_program(ldd NAMES ldd)
#   find_program(myTool NAMES myTool)
#
#   my_modules_do_stuff(
#     ...
#     LDD_PATH ldd
#     MY_TOOL_PATH myTool
#   )
#
# Providing a helper function that finds required tools can also help::
#
#   find_package(MyCMakeModules REQUIRED)
#   my_modules_find_required_tools()
#   # On Linux, ldd and myTool are defined
#   # On Windows, objdump and myTool are defined
#
#   my_modules_do_stuff(
#     ...
#     LDD_PATH ldd
#     MY_TOOL_PATH myTool
#   )
#
#
#
# Subtitle
# ^^^^^^^^
#
# TODO: should go to MdtPackageConfigHelpers
#
#  - :command:`mdt_xxxx()`
#  - :command:`mdt_xxxxs()`
#
#
# .. command:: mdt_xxxx
#
# Get find_program() command to find a tool::
#
#   mdt_xxxx(
#     <VAR>
#     TOOL name
#   )
#
#
# Example:
#
# .. code-block:: cmake
#
#   mdt_xxxx(
#     findMyToolCommandString
#     TOOL myTool
#   )
#
# For this example, ``findMyToolCommandString``
# will contain a command like::
#
#   find_program(toolPath NAMES myTool)
#   if(NOT toolPath)
#     message(FATAL_ERROR "...")
#   endif()
#   add_executable(myTool IMPORTED)
#   set_target_properties(myTool PROPERTIES IMPORTED_LOCATION "${toolPath}")
#
#
# .. command:: mdt_xxxxs
#
# Get find_program() command to find a list of tools::
#
#   mdt_xxxxs(
#   )
#
#
# Example:
#
# .. code-block:: cmake
#
#   mdt_xxxxs(
#   )
#
#
# 
#
#


# include(CMakePackageConfigHelpers)

function(mdt_install_cmake_modules)

  set(options NO_PACKAGE_CONFIG_FILES)
  set(oneValueArgs EXPORT_NAME INSTALL_NAMESPACE INSTALL_IS_UNIX_SYSTEM_WIDE COMPONENT)
  set(multiValueArgs FILES TOOLS LINUX_TOOLS WINDOWS_TOOLS APPLE_TOOLS)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_FILES)
    message(FATAL_ERROR "mdt_install_cmake_modules(): at least 1 file to a module must be provided")
  endif()
  if(NOT ARG_EXPORT_NAME)
    message(FATAL_ERROR "mdt_install_cmake_modules(): EXPORT_NAME missing")
  endif()
  if(NOT ARG_INSTALL_NAMESPACE)
    message(FATAL_ERROR "mdt_install_cmake_modules(): INSTALL_NAMESPACE missing")
  endif()

  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "mdt_install_cmake_modules(): unknown arguments passed: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT CMAKE_INSTALL_DATADIR)
    message(FATAL_ERROR "mdt_install_cmake_modules(): CMAKE_INSTALL_DATADIR is not defined. Please include GNUInstallDirs before calling this function")
  endif()

  set(packageName "${ARG_INSTALL_NAMESPACE}${ARG_EXPORT_NAME}")
  
  message("packageName: ${packageName}")

  if(ARG_INSTALL_IS_UNIX_SYSTEM_WIDE)
    set(modulesInstallDir "${CMAKE_INSTALL_DATADIR}/${packageName}/Modules")
    set(packageConfigInstallDir "${CMAKE_INSTALL_DATADIR}/cmake/${packageName}")
  else()
    set(modulesInstallDir "cmake/Modules")
    set(packageConfigInstallDir "cmake/${packageName}")
  endif()

  message("modulesInstallDir: ${modulesInstallDir}")
  message("packageConfigInstallDir: ${packageConfigInstallDir}")

  set(componentArguments)
  if(ARG_COMPONENT)
    set(componentArguments COMPONENT ${ARG_COMPONENT})
  endif()

  install(
    FILES ${ARG_FILES}
    DESTINATION "${modulesInstallDir}"
    ${componentArguments}
  )
  
  # TODO: maybe warn if user passes TOOLS and NO_PACKAGE_CONFIG_FILES

  if(NOT ARG_NO_PACKAGE_CONFIG_FILES)
    set(packageConfigFileInContent "@PACKAGE_INIT@\n\n")
    string(APPEND packageConfigFileInContent "set(MDT_CMAKE_MODULE_PATH \"@PACKAGE_modulesInstallDir@\")\n\n")
    string(APPEND packageConfigFileInContent "# Add to CMAKE_MODULE_PATH if not allready\n")
    string(APPEND packageConfigFileInContent "if(NOT \"\${MDT_CMAKE_MODULE_PATH}\" IN_LIST CMAKE_MODULE_PATH)\n")
    string(APPEND packageConfigFileInContent "  list(APPEND CMAKE_MODULE_PATH \"\${MDT_CMAKE_MODULE_PATH}\")\n")
    string(APPEND packageConfigFileInContent "endif()\n")

    set(packageConfigFileIn "${CMAKE_CURRENT_BINARY_DIR}/${packageName}Config.cmake.in")
    set(packageConfigFile "${CMAKE_CURRENT_BINARY_DIR}/${packageName}Config.cmake")

    file(WRITE "${packageConfigFileIn}" "${packageConfigFileInContent}")

    configure_package_config_file(
      "${packageConfigFileIn}"
      "${packageConfigFile}"
      INSTALL_DESTINATION "${packageConfigInstallDir}"
      PATH_VARS modulesInstallDir
      NO_SET_AND_CHECK_MACRO
      NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    install(
      FILES ${packageConfigFile}
      DESTINATION "${packageConfigInstallDir}"
      ${componentArguments}
    )
  endif()

endfunction()
